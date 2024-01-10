#include "nonblock.h"

void fd_set_nb(int fd) {
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if (errno) {
        perror("fcntl error");
        return;
    }

    flags |= O_NONBLOCK;

    errno = 0;
    (void) fcntl(fd, F_SETFL, flags);
    if (errno) {
        perror("fcntl error");
    }
}

struct Conn* new_conn(int fd){
    struct Conn* conn = malloc(sizeof(struct Conn));
    if (conn){
        conn->fd = fd;
        conn->rbuf_size = 0;
        conn->wbuf_size = 0;
        conn->wbuf_sent = 0;
    } else {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

void epollin_part(struct Conn* conn){
    uint8_t* text = read_text(conn);
    if (!text) return;
    uint8_t* out = do_request(text);
    size_t len = fill_buf(conn->wbuf, out);
    conn->wbuf_size = len;
    free(out);
}

void epollout_part(struct Conn* conn){
    write_all(conn->fd, conn->wbuf, conn->wbuf_size);
    conn->wbuf_size = 0;
}

void add_new_fd(int epollfd, int fd, struct Conn** fd2conn){
    fd2conn[fd] = new_conn(fd);
    struct epoll_event evt_new = {.events = EPOLLIN | EPOLLOUT | EPOLLET, .data.fd = fd};
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &evt_new);
}

bool epoll_wait_wrapper(int epollfd, struct epoll_event *evt){
    if (epoll_wait(epollfd, evt, MAX_EVENTS, TIMEOUT) < 1){
        if (errno == EINTR){
            return false;
        }
        perror("select");
        exit(EXIT_FAILURE);
    }
    return true;
}

uint8_t* read_text(struct Conn* conn){
    int len;
    ssize_t rh = read(conn->fd, &len, HEADER_SIZE);
    ssize_t rb = read_full(conn->fd, conn->rbuf, len);

    if (rh == -1 || rb == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)){
        perror("read");
        close(conn->fd);
        conn = NULL;
        return NULL;
    }

    conn->rbuf_size = len;
    conn->rbuf[conn->rbuf_size] = '\0';
    return conn->rbuf;
}

int buf_size(uint8_t* buf){
    int len;
    memcpy(&len, buf, HEADER_SIZE);
    return len;
}

size_t fill_buf(uint8_t* dist, uint8_t* src){
    size_t len = buf_size(src) + HEADER_SIZE;
    memcpy(dist, src, len);
    return len;
}