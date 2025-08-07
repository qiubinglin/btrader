# Run in Orbstack
## In host
1. Install XQuartz and allow connection from internet client.
2. Run `xhost + 127.0.0.1`

## In Orbstack
```
export DISPLAY=host.docker.internal:0
export QT_LOGGING_RULES="qt.qpa.*=false"
export QT_QUICK_BACKEND=software
```