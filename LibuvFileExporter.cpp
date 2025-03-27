#include "LibuvFileExporter.h"
#include <QMetaObject>
#include <QDebug>

LibuvFileExporter::LibuvFileExporter(QObject* parent) : QObject(parent) {}
LibuvFileExporter::~LibuvFileExporter() {}

void LibuvFileExporter::reportProgress(int percent) {
    QMetaObject::invokeMethod(this, [this, percent]() {
        if (_context && _context->onProgress) _context->onProgress(percent);
    }, Qt::QueuedConnection);
}

void LibuvFileExporter::reportComplete() {
    QMetaObject::invokeMethod(this, [this]() {
        if (_context && _context->onComplete) _context->onComplete();
    }, Qt::QueuedConnection);
}

void LibuvFileExporter::startWrite() {
    if (!_context.has_value()) {
        qWarning() << "❗ [startWrite] _context is empty!";
        return;
    }

    uv_loop_t* loop = uv_default_loop();
    _context->open_req.data = &*_context;
    uv_fs_open(loop, &_context->open_req,
               _context->path.toUtf8().constData(),
               O_WRONLY | O_CREAT | O_TRUNC, 0644, onOpen);
    loop->data = &*_context;
}

void LibuvFileExporter::onOpen(uv_fs_t* req) {
    auto* ctx = static_cast<WriteContext*>(req->data);
    if (!ctx) {
        qWarning() << "❗ [onOpen] ctx is null";
        return;
    }
    if (req->result < 0) {
        qWarning() << "[libuv] 파일 열기 실패:" << uv_strerror(static_cast<int>(req->result));
        return;
    }
    ctx->file = static_cast<uv_file>(req->result);
    onWrite(nullptr);
}

void LibuvFileExporter::onWrite(uv_fs_t*) {
    auto* ctx = static_cast<WriteContext*>(uv_default_loop()->data);
    if (!ctx) {
        qWarning() << "❗ [onWrite] ctx is null";
        return;
    }
    if (ctx->index >= ctx->lines.size()) {
        qDebug() << "📦 파일 모두 작성 완료 → uv_fs_close 진입 예정";
        ctx->close_req.data = ctx;
        uv_fs_close(uv_default_loop(), &ctx->close_req, ctx->file, onClose);
        return;
    }

    QByteArray& line = ctx->lines[ctx->index++];
    uv_buf_t buf = uv_buf_init(line.data(), line.size());
    ctx->write_req.data = ctx;
    uv_fs_write(uv_default_loop(), &ctx->write_req, ctx->file, &buf, 1, -1, onWrite);
    if (ctx->onProgress) ctx->onProgress(ctx->index * 100 / ctx->lines.size());
}

void LibuvFileExporter::onClose(uv_fs_t* req) {
    auto* ctx = static_cast<WriteContext*>(req->data);
    if (!ctx) {
        qWarning() << "❗ [onClose] ctx is null";
        return;
    }

    qDebug() << "🔥 onClose 진입됨";
    if (ctx->onProgress) ctx->onProgress(100);

    if (!ctx->onComplete) {
        qWarning() << "❌ onComplete가 nullptr입니다!";
    } else {
        qDebug() << "✅ onComplete 호출됨";
        ctx->onComplete();
    }

    uv_fs_req_cleanup(req);
}

template<typename T>
void LibuvFileExporter::writeCsvAsync(const QVector<T>& data,
                                      const QString& filePath,
                                      std::function<QString(const T&)> formatter,
                                      std::function<void(int)> onProgress,
                                      std::function<void()> onComplete) {
    _context.reset();
    _context = std::optional<WriteContext>{};

    if (!_context.has_value()) {
        qWarning() << "❗ [writeCsvAsync] _context 생성 실패";
        return;
    }

    _context->path = filePath;
    _context->onProgress = std::move(onProgress);
    _context->onComplete = std::move(onComplete);

    for (const T& item : data)
        _context->lines.append(formatter(item).toUtf8() + "\n");

    startWrite();
}

template void LibuvFileExporter::writeCsvAsync<int>(const QVector<int>&, const QString&, std::function<QString(const int&)>, std::function<void(int)>, std::function<void()>);
