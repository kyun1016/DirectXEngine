

namespace kyun {

    inline void ThrowIfFailed(HRESULT hr) {
        if (FAILED(hr)) {
            // ������� �� ���⿡ breakpoint ����
            throw std::exception();
        }
    }
}