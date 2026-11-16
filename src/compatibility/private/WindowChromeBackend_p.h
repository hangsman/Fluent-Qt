#pragma once

#include "compatibility/WindowChromeCompat.h"

namespace compatibility::detail {

// Every platform implements this internal backend contract.
void applyPlatformWindowFlags(QWidget* window, const WindowChromeOptions& options);
bool handlePlatformNativeEvent(QWidget* window,
    const WindowChromeOptions& options,
    const QByteArray& eventType,
    void* message,
    FluentNativeEventResult* result);
bool beginPlatformSystemMove(QWidget* window, const QPoint& globalPos);
bool beginPlatformSystemResize(QWidget* window, Qt::Edges edges, const QPoint& globalPos);
bool performPlatformTitleBarDoubleClick(QWidget* window, const WindowChromeOptions& options);
bool showPlatformSystemMenu(QWidget* window, const QPoint& globalPos);
bool requestPlatformForegroundActivation(QWidget* window);
// Arms the native caption that Windows needs to start its window transition.
// The platform releases it when the state change, show, or hide settles. The
// window keeps its client-side frame and client rectangle while armed.
// zh_CN: 为 Windows 启动窗口过渡预先补上原生 caption，平台在状态变化、显示或隐藏落定后
// 自动释放；期间窗口仍保持客户端边框与客户区矩形。
void beginPlatformWindowTransition(QWidget* window);
// Hides a visible top-level window while the application leaves its event
// loop, because quitting destroys native windows without a close event and the
// platform would otherwise skip the close transition.
// zh_CN: 应用离开事件循环时隐藏可见顶层窗口；直接退出会销毁原生窗口而不产生 close 事件，
// 平台因此会跳过关闭过渡。
void installPlatformWindowQuitTransition(QWidget* window);
// Reapplies the maximized frame after the window manager constrained a window
// shown maximized; returns true when the frame changed.
// zh_CN: 窗口管理器约束了以最大化显示的窗口后，重新应用最大化框架；框架改变时返回 true。
bool restorePlatformMaximizedFrame(QWidget* window);
void syncPlatformTitleBarGeometry(QWidget* window, const WindowChromeOptions& options);
int nativeTitleBarLeadingInset(QWidget* window);
int clientSideFrameMargin(QWidget* window, const WindowChromeOptions& options);
bool manualMoveResizeFallbackAllowed(QWidget* window, const WindowChromeOptions& options);
BackdropCapabilities platformBackdropCapabilities();
BackdropApplyResult
applyPlatformSystemBackdrop(QWidget* window, BackdropEffect effect, bool dark, bool forceRecomposite);
bool flushPlatformWindowBackdropSurface(QWidget* window);
void preparePlatformWindowBackdropSurface(QWidget* window,
    const QColor& opaqueColor,
    fluent::windowing::BackdropSurfaceMode mode);

} // namespace compatibility::detail
