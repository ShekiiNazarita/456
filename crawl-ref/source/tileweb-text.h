#ifdef USE_TILE_WEB
#ifndef TILEWEB_TEXT_H
#define TILEWEB_TEXT_H

#include <string>

class WebTextArea
{
public:
    WebTextArea(std::string name);
    virtual ~WebTextArea();

    void resize(int mx, int my);

    void clear();

    void put_character(ucs_t chr, int fg, int bg, int x, int y);

    void send();

    int mx, my; // Size

protected:
    ucs_t   *m_cbuf; // Character buffer
    uint8_t *m_abuf; // Color buffer

    std::string m_client_side_name;

    virtual void on_resize();
};

class CRTTextArea : public WebTextArea
{
public:
    CRTTextArea();

protected:
    virtual void on_resize();
};

class StatTextArea : public WebTextArea
{
public:
    StatTextArea();

protected:
    virtual void on_resize();
};

class MessageTextArea : public WebTextArea
{
public:
    MessageTextArea();

protected:
    virtual void on_resize();
};

#endif
#endif
