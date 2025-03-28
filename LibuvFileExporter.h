/*
 * LibuvFileExporter v1.0
 * ------------------------
 * 버전 특징:
 * - ✅ libuv 기반: OS 레벨 비동기 파일 I/O
 * - ✅ Qt UI와 완전 분리된 진짜 비동기 처리 (UI 간섭 0%)
 * - ✅ 스레드/멀티프로세스 사용하지 않음 (libuv 내부 루프만 사용)
 * - ✅ CSV 저장 지원 (formatter 함수 기반)
 * - ✅ 진행률 콜백 및 완료 콜백 제공
 * - ✅ 자동 파일 열기/닫기, 오류 감지
 * - ✅ Qt에서 안전하게 사용 가능 (invokeMethod로 콜백 전달)
 * - ✅ 대용량 파일 저장 최적화, 이벤트 기반 I/O 처리
 */

#ifndef LIBUV_FILE_EXPORTER_H
#define LIBUV_FILE_EXPORTER_H

#ifdef _WIN32
#include <winsock2.h>  // ✅ 반드시 windows.h보다 먼저
#endif

#include <QObject>
#include <QString>
#include <QVector>
#include <functional>
#include <uv.h>
#include <optional>

class LibuvFileExporter : public QObject {
    Q_OBJECT
public:
    explicit LibuvFileExporter(QObject* parent = nullptr);
    ~LibuvFileExporter();

    template<typename T>
    void writeCsvAsync(const QVector<T>& data,
                       const QString& filePath,
                       std::function<QString(const T&)> formatter,
                       std::function<void(int)> onProgress = {},
                       std::function<void()> onComplete = {});

    bool isInProgress() const { return _context.has_value(); }

private:
    struct WriteContext {
        WriteContext() = default;

        QVector<QByteArray> lines;
        int index = 0;
        uv_fs_t open_req;
        uv_fs_t write_req;
        uv_fs_t close_req;
        uv_file file = -1;
        QString path;
        std::function<void(int)> onProgress;
        std::function<void()> onComplete;

        QObject* qobject = nullptr;  // ✅ 이 줄 추가
        LibuvFileExporter* owner = nullptr;  // ✅ 이 줄 추가!
    };

    std::optional<WriteContext> _context;

    void startWrite();
    static void onOpen(uv_fs_t* req);
    static void onWrite(uv_fs_t* req);
    static void onClose(uv_fs_t* req);

    void reportProgress(int percent);
    void reportComplete();
};

#endif // LIBUV_FILE_EXPORTER_H
