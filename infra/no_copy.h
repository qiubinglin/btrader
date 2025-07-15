#pragma once

class NoCopy {
public:
    NoCopy(const NoCopy&) = delete;
    void operator=(const NoCopy&) = delete;

protected:
    NoCopy() = default;
    virtual ~NoCopy() = default;
};

