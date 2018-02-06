#include "kabufuda/AsyncIO.hpp"

namespace kabufuda
{

AsyncIO::AsyncIO(SystemStringView filename, bool truncate)
{
    m_fd = open(filename.data(), O_RDWR | O_CREAT | (truncate ? O_TRUNC : 0));
}

AsyncIO::~AsyncIO()
{
    if (*this)
    {
        aio_cancel(m_fd, nullptr);
        close(m_fd);
    }
}

AsyncIO::AsyncIO(AsyncIO&& other)
{
    m_fd = other.m_fd;
    other.m_fd = -1;
    m_queue = std::move(other.m_queue);
    m_maxBlock = other.m_maxBlock;
}

AsyncIO& AsyncIO::operator=(AsyncIO&& other)
{
    if (*this)
    {
        aio_cancel(m_fd, nullptr);
        close(m_fd);
    }
    m_fd = other.m_fd;
    other.m_fd = -1;
    m_queue = std::move(other.m_queue);
    m_maxBlock = other.m_maxBlock;
    return *this;
}

SizeReturn AsyncIO::syncRead(void* buf, size_t length, off_t offset)
{
    lseek(m_fd, offset, SEEK_SET);
    return read(m_fd, buf, length);
}

bool AsyncIO::asyncRead(size_t qIdx, void* buf, size_t length, off_t offset)
{
    struct aiocb& aio = m_queue[qIdx].first;
    if (aio.aio_fildes)
    {
#ifndef NDEBUG
        fprintf(stderr, "WARNING: synchronous kabufuda fallback, check access polling\n");
#endif
        const struct aiocb* aiop = &aio;
        struct timespec ts = {2, 0};
        while (aio_suspend(&aiop, 1, &ts) && errno == EINTR) {}
        if (aio_error(&aio) == 0)
            aio_return(&aio);
    }
    memset(&aio, 0, sizeof(struct aiocb));
    aio.aio_fildes = m_fd;
    aio.aio_offset = offset;
    aio.aio_buf = buf;
    aio.aio_nbytes = length;
    m_maxBlock = std::max(m_maxBlock, qIdx + 1);
    return aio_read(&aio) == 0;
}

SizeReturn AsyncIO::syncWrite(const void* buf, size_t length, off_t offset)
{
    lseek(m_fd, offset, SEEK_SET);
    return write(m_fd, buf, length);
}

bool AsyncIO::asyncWrite(size_t qIdx, const void* buf, size_t length, off_t offset)
{
    struct aiocb& aio = m_queue[qIdx].first;
    if (aio.aio_fildes)
    {
#ifndef NDEBUG
        fprintf(stderr, "WARNING: synchronous kabufuda fallback, check access polling\n");
#endif
        const struct aiocb* aiop = &aio;
        struct timespec ts = {2, 0};
        while (aio_suspend(&aiop, 1, &ts) && errno == EINTR) {}
        if (aio_error(&aio) == 0)
            aio_return(&aio);
    }
    memset(&aio, 0, sizeof(struct aiocb));
    aio.aio_fildes = m_fd;
    aio.aio_offset = offset;
    aio.aio_buf = const_cast<void*>(buf);
    aio.aio_nbytes = length;
    m_maxBlock = std::max(m_maxBlock, qIdx + 1);
    return aio_write(&aio) == 0;
}

ECardResult AsyncIO::pollStatus(size_t qIdx, SizeReturn* szRet) const
{
    auto& aio = const_cast<AsyncIO*>(this)->m_queue[qIdx];
    if (aio.first.aio_fildes == 0)
    {
        if (szRet)
            *szRet = aio.second;
        return ECardResult::READY;
    }
    switch (aio_error(&aio.first))
    {
    case 0:
        aio.second = aio_return(&aio.first);
        aio.first.aio_fildes = 0;
        if (szRet)
            *szRet = aio.second;
        return ECardResult::READY;
    case EINPROGRESS:
        return ECardResult::BUSY;
    default:
        return ECardResult::IOERROR;
    }
}

ECardResult AsyncIO::pollStatus() const
{
    ECardResult result = ECardResult::READY;
    for (auto it = const_cast<AsyncIO*>(this)->m_queue.begin();
         it != const_cast<AsyncIO*>(this)->m_queue.begin() + m_maxBlock;
         ++it)
    {
        auto& aio = *it;
        if (aio.first.aio_fildes == 0)
            continue;
        switch (aio_error(&aio.first))
        {
        case 0:
            aio.second = aio_return(&aio.first);
            aio.first.aio_fildes = 0;
            break;
        case EINPROGRESS:
            if (result > ECardResult::BUSY)
                result = ECardResult::BUSY;
            break;
        default:
            if (result > ECardResult::IOERROR)
                result = ECardResult::IOERROR;
            break;
        }
    }
    if (result == ECardResult::READY)
        const_cast<AsyncIO*>(this)->m_maxBlock = 0;
    return result;
}

void AsyncIO::waitForCompletion() const
{
    for (auto it = const_cast<AsyncIO*>(this)->m_queue.begin();
         it != const_cast<AsyncIO*>(this)->m_queue.begin() + m_maxBlock;
         ++it)
    {
        auto& aio = *it;
        if (aio.first.aio_fildes == 0)
            continue;
        switch (aio_error(&aio.first))
        {
        case 0:
            aio.second = aio_return(&aio.first);
            aio.first.aio_fildes = 0;
            break;
        case EINPROGRESS:
        {
            const struct aiocb* aiop = &aio.first;
            struct timespec ts = {2, 0};
            while (aio_suspend(&aiop, 1, &ts) && errno == EINTR) {}
            break;
        }
        default:
            break;
        }
    }
}

}
