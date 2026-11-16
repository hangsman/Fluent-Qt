#include "GalleryFrameRateLabel.h"

#include <QApplication>
#include <QEvent>
#include <QHideEvent>
#include <QPainter>
#include <QShowEvent>
#include <QTimer>

namespace fluent::gallery {
namespace {

constexpr int kSampleIntervalMs = 500;

} // namespace

GalleryFrameRateLabel::GalleryFrameRateLabel(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("GalleryTitleBar.FrameRate"));
    setAccessibleName(QStringLiteral("Frame rate"));
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_sampleTimer = new QTimer(this);
    m_sampleTimer->setObjectName(QStringLiteral("galleryFrameRateSampleTimer"));
    m_sampleTimer->setInterval(kSampleIntervalMs);
    connect(m_sampleTimer, &QTimer::timeout, this, &GalleryFrameRateLabel::sampleFrames);
    updateAccessibleDescription();
}

GalleryFrameRateLabel::~GalleryFrameRateLabel()
{
    if (m_measuring)
        qApp->removeEventFilter(this);
}

void GalleryFrameRateLabel::setDisplayEnabled(bool enabled)
{
    if (m_displayEnabled == enabled)
        return;
    m_displayEnabled = enabled;
    refreshMeasurement();
}

void GalleryFrameRateLabel::setMonitoredRoot(QWidget* root)
{
    m_monitoredRoot = root;
    resetMeasurement();
}

void GalleryFrameRateLabel::onThemeUpdated()
{
    update();
}

bool GalleryFrameRateLabel::eventFilter(QObject* watched, QEvent* event)
{
    if (!m_measuring || !m_monitoredRoot || event->type() != QEvent::Paint)
        return QWidget::eventFilter(watched, event);

    auto* painted = qobject_cast<QWidget*>(watched);
    if (!painted || (painted != m_monitoredRoot && !m_monitoredRoot->isAncestorOf(painted)))
        return QWidget::eventFilter(watched, event);

    // One backing-store update paints several dirty widgets. Commit only after
    // the current event-loop batch so those widgets still count as one frame.
    // zh_CN: 一次后备缓冲更新会绘制多个脏控件；等当前事件循环批次结束后再计数，确保它们只算一帧。
    if (!m_framePending) {
        m_framePending = true;
        QTimer::singleShot(0, this, [this]() {
            if (!m_framePending)
                return;
            m_framePending = false;
            ++m_framesInSample;
        });
    }
    return QWidget::eventFilter(watched, event);
}

void GalleryFrameRateLabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto& colors = themeColorsRef();
    painter.setPen(QPen(colors.strokeDefault, 1.0));
    painter.setBrush(colors.controlDefault);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), themeRadius().control,
        themeRadius().control);

    painter.setFont(themeFont(Typography::FontRole::Caption).toQFont());
    painter.setPen(colors.textSecondary);
    painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("%1 FPS").arg(m_frameRate));
}

void GalleryFrameRateLabel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    refreshMeasurement();
}

void GalleryFrameRateLabel::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    refreshMeasurement();
}

void GalleryFrameRateLabel::refreshMeasurement()
{
    const bool shouldMeasure = m_displayEnabled && isVisible();
    if (shouldMeasure == m_measuring)
        return;

    m_measuring = shouldMeasure;
    if (m_measuring) {
        resetMeasurement();
        m_sampleClock.start();
        qApp->installEventFilter(this);
        m_sampleTimer->start();
        return;
    }

    m_sampleTimer->stop();
    qApp->removeEventFilter(this);
    resetMeasurement();
}

void GalleryFrameRateLabel::sampleFrames()
{
    const qint64 elapsed = m_sampleClock.restart();
    const int frames = m_framesInSample;
    m_framesInSample = 0;
    m_framePending = false;
    const int nextRate = elapsed > 0 ? qRound(frames * 1000.0 / elapsed) : 0;
    if (nextRate == m_frameRate)
        return;
    m_frameRate = nextRate;
    updateAccessibleDescription();
    update();
}

void GalleryFrameRateLabel::resetMeasurement()
{
    m_framesInSample = 0;
    m_framePending = false;
    m_frameRate = 0;
    m_sampleClock.invalidate();
    updateAccessibleDescription();
    update();
}

void GalleryFrameRateLabel::updateAccessibleDescription()
{
    setAccessibleDescription(QStringLiteral("Current painting rate: %1 frames per second").arg(m_frameRate));
}

} // namespace fluent::gallery
