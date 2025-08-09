# TradeView (Qt6/QML GUI)

Interactive trading charts and order flow views. Requires a working X11 display (Linux) or XQuartz (macOS via Orbstack).

## Quick start (macOS + Orbstack)

### On macOS host
1) Install XQuartz (`brew install --cask xquartz`) and launch it once.
2) Allow local clients:
```bash
xhost + 127.0.0.1
```

### In Orbstack container
```bash
export DISPLAY=host.docker.internal:0
export QT_LOGGING_RULES="qt.qpa.*=false"
# Optional: force software rendering if GPU accel is not available
export QT_QUICK_BACKEND=software
```

## Linux notes
- On X11: ensure `$DISPLAY` is set (e.g., `:0`) and X server is running.
- On Wayland: consider `QT_QPA_PLATFORM=xcb` if you see rendering issues.

## Run
After building the project from the repo root:
```bash
cd build
./tradeview_app   # replace with actual TradeView binary name if different
```

## Features
- Candlestick + volume charts with zoom/pan and crosshair
- Time axis with adaptive ticks and date markers
- Order book and order flow views
- Debounced repainting for smooth interaction

## Performance tips
- Prefer Release builds
- Set `QT_QUICK_BACKEND=software` if the GPU path is unstable
- Avoid excessive logging in hot paths

## Troubleshooting
- Blank window or slow UI: try software backend or update graphics drivers
- No display: verify `$DISPLAY` and X server/XQuartz; check firewall rules