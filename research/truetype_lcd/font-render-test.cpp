#include <new>
#include <stdio.h>

#include "SDL_error.h"
#include "SDL_log.h"
#include "SDL_surface.h"

#include "font_render_lcd.h"

extern "C" int main(int argc, char *argv[]);

#if defined(_WIN32) || defined(WIN32)
static const char *os_font_directory = "C:/Windows/fonts";
static const char *font_filename = "georgia.ttf";
#else
static const char *os_font_directory = "/usr/share/fonts/truetype";
const char *font_filename = "liberation/LiberationSerif-Regular.ttf";
#endif

static char *path_join(const char *dir, const char *filename) {
    const int path_len = strlen(dir), filename_len = strlen(filename);
    const int len = path_len + 1 + filename_len + 1;
    char *full_name = new char[len];
    memcpy(full_name, dir, path_len);
    memcpy(full_name + path_len, "/", 1);
    memcpy(full_name + path_len + 1, filename, filename_len);
    full_name[len - 1] = 0;
    return full_name;
}

int main(int argc, char* argv[])
{
    if (argc > 2) {
        fprintf(stderr, "usage: %s [truetype-font-dir]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        os_font_directory = argv[1];
    }
    fprintf(stderr, "looking for truetype fonts in directory %s.\n", os_font_directory);

    const char *font_full_path = path_join(os_font_directory, font_filename);

    agg::rendering_buffer ren_buf;
    const unsigned width = 640, height = 640;
    const unsigned pixel_size = 3;
    agg::int8u *buffer_data = new(std::nothrow) agg::int8u[width * height * pixel_size];
    if (!buffer_data) {
        fprintf(stderr, "Cannor allocate memory for buffer.\n");
        return 1;
    }
    ren_buf.attach(buffer_data, width, height, -width * pixel_size);

    font_renderer_lcd font_renderer;
    font_renderer.clear(ren_buf, agg::rgba8(0xe9, 0xe5, 0xcd));

    font_renderer.render_text(ren_buf, font_full_path,
        24, agg::rgba8(0x23, 0x1f, 0x20), 20, 320, "Hello world!");

    Uint32 pixel_format = SDL_PIXELFORMAT_RGB24;
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormatFrom((void *)buffer_data, width, height,
        pixel_size * 8, pixel_size * width, pixel_format);

    if (SDL_SaveBMP(surf, "output.bmp") < 0) {
        const char *error_message = SDL_GetError();
        SDL_Log("SDL Error writing bmp file \"output.bmp\": %s\n", error_message);
        return 1;
    }

    return 0;
}
