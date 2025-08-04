#include "workthread.h"

void WorkThread::start() { QThread::start(); }

void WorkThread::terminate() { QThread::terminate(); }

void WorkThread::quit() { QThread::quit(); }

bool WorkThread::wait() { return QThread::wait(); }