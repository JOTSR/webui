/*
  WebUI Library 2.3.0
  http://_webui_core.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

// -- Third-party ---------------------
#define MG_BUF_LEN (1024 * 16)
#include "civetweb/civetweb.h"

// -- WebUI ---------------------------
#include "webui_core.h"

// -- WebUI JS API --------------------
#include "client/webui.h"
static char *webui_javascript_bridge = client_webui_js;

// -- Heap ----------------------------
static _webui_core_t _webui_core;

// -- Heap ----------------------------
static const char *webui_html_served =
    "<html><head><title>Access "
    "Denied</"
    "title><style>body{margin:0;background-repeat:no-repeat;background-"
    "attachment:fixed;background-color:#FF3CAC;background-image:linear-"
    "gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 "
    "100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</"
    "style></head><body><h2>&#9888; Access Denied</h2><p>You can't access this "
    "content<br>because it's already processed.<br><br>The current security "
    "policy denies<br>multiple requests.</p><br><a "
    "href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION
    "<small></a></body></html>";
static const char *webui_html_res_not_available =
    "<html><head><title>Resource Not "
    "Available</"
    "title><style>body{margin:0;background-repeat:no-repeat;background-"
    "attachment:fixed;background-color:#FF3CAC;background-image:linear-"
    "gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 "
    "100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</"
    "style></head><body><h2>&#9888; Resource Not Available</h2><p>The "
    "requested resource is not available.</p><br><a "
    "href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION
    "<small></a></body></html>";
static const char *webui_deno_not_found =
    "<html><head><title>Deno Not "
    "Found</"
    "title><style>body{margin:0;background-repeat:no-repeat;background-"
    "attachment:fixed;background-color:#FF3CAC;background-image:linear-"
    "gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 "
    "100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</"
    "style></head><body><h2>&#9888; Deno Not Found</h2><p>Deno is not found on "
    "this system.<br>Please download it from <a "
    "href=\"https://github.com/denoland/deno/releases\">https://github.com/"
    "denoland/deno/releases</a></p><br><a "
    "href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION
    "<small></a></body></html>";
static const char *webui_nodejs_not_found =
    "<html><head><title>Node.js Not "
    "Found</"
    "title><style>body{margin:0;background-repeat:no-repeat;background-"
    "attachment:fixed;background-color:#FF3CAC;background-image:linear-"
    "gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 "
    "100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</"
    "style></head><body><h2>&#9888; Node.js Not Found</h2><p>Node.js is not "
    "found on this system.<br>Please download it from <a "
    "href=\"https://nodejs.org/en/download/\">https://nodejs.org/en/download/</"
    "a></p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION
    "<small></a></body></html>";
static const char *webui_def_icon =
    "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"64px\" "
    "height=\"64px\"><g><path style=\"opacity:0.946\" fill=\"#0dedfe\" d=\"M "
    "32.5,16.5 C 31.8535,18.1561 31.1869,19.8228 30.5,21.5C 30.7189,23.9153 "
    "32.0523,24.9153 34.5,24.5C 32.9249,28.0781 30.9249,31.4114 28.5,34.5C "
    "26.1391,38.5172 23.8058,42.5172 21.5,46.5C 16.7715,49.6539 "
    "12.7715,48.8206 9.5,44C 9.72845,43.6012 10.0618,43.4346 10.5,43.5C "
    "12.9032,43.607 14.7365,42.607 16,40.5C 17.4814,37.89 18.6481,35.2234 "
    "19.5,32.5C 21.6552,28.8609 23.8218,25.1943 26,21.5C 27.7581,19.2345 "
    "29.9248,17.5678 32.5,16.5 Z\"/></g><g><path style=\"opacity:0.927\" "
    "fill=\"#3cbdf2\" d=\"M 14.5,22.5 C 10.0664,23.8825 5.73309,25.5492 "
    "1.5,27.5C -1.03001,19.4231 1.96999,15.9231 10.5,17C 12.4641,18.451 "
    "13.7974,20.2843 14.5,22.5 Z\"/></g><g><path style=\"opacity:1\" "
    "fill=\"#8df3ff\" d=\"M 42.5,23.5 C 39.7697,24.2073 37.103,24.5406 "
    "34.5,24.5C 32.0523,24.9153 30.7189,23.9153 30.5,21.5C 31.1869,19.8228 "
    "31.8535,18.1561 32.5,16.5C 37.0039,13.261 41.6705,12.9277 46.5,15.5C "
    "48.3325,19.6833 46.9991,22.35 42.5,23.5 Z\"/></g><g><path "
    "style=\"opacity:1\" fill=\"#13dcf8\" d=\"M 42.5,23.5 C 42.0679,24.7095 "
    "41.4013,25.7095 40.5,26.5C 37.4531,28.9181 35.1198,31.9181 33.5,35.5C "
    "31.8828,34.962 30.2161,34.6287 28.5,34.5C 30.9249,31.4114 32.9249,28.0781 "
    "34.5,24.5C 37.103,24.5406 39.7697,24.2073 42.5,23.5 Z\"/></g><g><path "
    "style=\"opacity:0.922\" fill=\"#5b75db\" d=\"M 58.5,34.5 C "
    "54.0469,32.7737 49.7136,30.7737 45.5,28.5C 47.0043,24.1422 "
    "49.3376,20.3088 52.5,17C 58.6903,15.419 62.0236,17.7524 62.5,24C "
    "61.8235,27.8357 60.4902,31.3357 58.5,34.5 Z\"/></g><g><path "
    "style=\"opacity:1\" fill=\"#29a7e8\" d=\"M 14.5,22.5 C 15.5,24.5 "
    "16.5,26.5 17.5,28.5C 13.2909,30.7712 8.95759,32.7712 4.5,34.5C "
    "3.26377,32.351 2.26377,30.0177 1.5,27.5C 5.73309,25.5492 10.0664,23.8825 "
    "14.5,22.5 Z\"/></g><g><path style=\"opacity:0.943\" fill=\"#158fdd\" "
    "d=\"M 17.5,28.5 C 17.9887,29.9952 18.6554,31.3285 19.5,32.5C "
    "18.6481,35.2234 17.4814,37.89 16,40.5C 14.7365,42.607 12.9032,43.607 "
    "10.5,43.5C 9.70951,42.5987 8.70951,41.9321 7.5,41.5C 6.22713,39.2872 "
    "5.22713,36.9538 4.5,34.5C 8.95759,32.7712 13.2909,30.7712 17.5,28.5 "
    "Z\"/></g><g><path style=\"opacity:1\" fill=\"#4f9be1\" d=\"M 40.5,26.5 C "
    "41.5,28.5 42.5,30.5 43.5,32.5C 43.5569,33.609 43.8902,34.609 44.5,35.5C "
    "40.7254,37.939 36.7254,39.939 32.5,41.5C 32.1667,41.5 31.8333,41.5 "
    "31.5,41.5C 31.6036,39.2972 32.2702,37.2972 33.5,35.5C 35.1198,31.9181 "
    "37.4531,28.9181 40.5,26.5 Z\"/></g><g><path style=\"opacity:1\" "
    "fill=\"#3868c9\" d=\"M 45.5,28.5 C 49.7136,30.7737 54.0469,32.7737 "
    "58.5,34.5C 57.7729,36.9538 56.7729,39.2872 55.5,41.5C 54.5,41.8333 "
    "53.8333,42.5 53.5,43.5C 50.6874,43.9754 48.6874,42.9754 47.5,40.5C "
    "46.2485,38.9991 45.2485,37.3325 44.5,35.5C 43.8902,34.609 43.5569,33.609 "
    "43.5,32.5C 44.3446,31.3285 45.0113,29.9952 45.5,28.5 Z\"/></g><g><path "
    "style=\"opacity:0.878\" fill=\"#19c3ed\" d=\"M 28.5,34.5 C "
    "30.2161,34.6287 31.8828,34.962 33.5,35.5C 32.2702,37.2972 31.6036,39.2972 "
    "31.5,41.5C 30.0787,44.0094 28.4121,46.3428 26.5,48.5C 24.8333,47.8333 "
    "23.1667,47.1667 21.5,46.5C 23.8058,42.5172 26.1391,38.5172 28.5,34.5 "
    "Z\"/></g><g><path style=\"opacity:1\" fill=\"#52d6fd\" d=\"M 47.5,40.5 C "
    "48.6874,42.9754 50.6874,43.9754 53.5,43.5C 51.5232,47.9704 "
    "48.1898,49.4704 43.5,48C 40.8065,43.7833 42.1398,41.2833 47.5,40.5 "
    "Z\"/></g><g><path style=\"opacity:0.912\" fill=\"#44bcf1\" d=\"M "
    "44.5,35.5 C 45.2485,37.3325 46.2485,38.9991 47.5,40.5C 42.1398,41.2833 "
    "40.8065,43.7833 43.5,48C 48.1898,49.4704 51.5232,47.9704 53.5,43.5C "
    "53.8333,42.5 54.5,41.8333 55.5,41.5C 53.8152,46.3707 51.4818,50.8707 "
    "48.5,55C 44.4087,56.2272 40.9087,55.3939 38,52.5C 36.1401,48.7802 "
    "34.3068,45.1136 32.5,41.5C 36.7254,39.939 40.7254,37.939 44.5,35.5 "
    "Z\"/></g><g><path style=\"opacity:0.877\" fill=\"#26a6e1\" d=\"M 7.5,41.5 "
    "C 8.70951,41.9321 9.70951,42.5987 10.5,43.5C 10.0618,43.4346 "
    "9.72845,43.6012 9.5,44C 12.7715,48.8206 16.7715,49.6539 21.5,46.5C "
    "23.1667,47.1667 24.8333,47.8333 26.5,48.5C 24.9942,55.0462 "
    "20.9942,57.2128 14.5,55C 11.503,50.8392 9.16965,46.3392 7.5,41.5 "
    "Z\"/></g></svg>";
static const char *webui_def_icon_type = "image/svg+xml";
static const char *webui_js_empty = "ERR_WEBUI_NO_SCRIPT_FOUND";
static const char *webui_js_timeout = "ERR_WEBUI_TIMEOUT";
static const char *const webui_empty_string =
    ""; // In case the compiler optimization is disabled

// -- Functions -----------------------
void webui_run(size_t window, const char *script) {

#ifdef WEBUI_LOG
  printf("[User] webui_run([%zu])...\n", window);
  printf("[User] webui_run([%zu]) -> Script: [%s]\n", window, script);
#endif

  size_t js_len = _webui_strlen(script);

  if (js_len < 1)
    return;

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  // Initializing pipe
  unsigned char run_id = _webui_get_run_id();
  _webui_core.run_done[run_id] = false;
  _webui_core.run_error[run_id] = false;
  if ((void *)_webui_core.run_responses[run_id] != NULL)
    _webui_free_mem((void *)_webui_core.run_responses[run_id]);

  // Prepare the packet
  size_t packet_len = 3 + js_len; // [header][js]
  char *packet = (char *)_webui_malloc(packet_len);
  packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
  packet[1] = WEBUI_HEADER_JS_QUICK;  // Type
  packet[2] = run_id;                 // ID
  for (size_t i = 0; i < js_len; i++) // Data
    packet[i + 3] = script[i];

  // Send packets
  _webui_window_send(win, packet, packet_len);
  _webui_free_mem((void *)packet);
}

void webui_set_file_handler(size_t window,
                            const void *(*handler)(const char *filename,
                                                   int *length)) {

  if (handler == NULL)
    return;

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  win->files_handler = handler;
}

bool webui_script(size_t window, const char *script, size_t timeout_second,
                  char *buffer, size_t buffer_length) {

#ifdef WEBUI_LOG
  printf("[User] webui_script([%zu])...\n", window);
  printf("[User] webui_script([%zu]) -> Script [%s] \n", window, script);
  printf("[User] webui_script([%zu]) -> Response Buffer @ 0x%p \n", window,
         buffer);
  printf("[User] webui_script([%zu]) -> Response Buffer Size %zu bytes \n",
         window, buffer_length);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return false;
  _webui_window_t *win = _webui_core.wins[window];

  _webui_init();

  // Initializing response buffer
  if (buffer_length > 0)
    memset(buffer, 0, buffer_length);

  size_t js_len = _webui_strlen(script);

  if (js_len < 1) {

    if (buffer != NULL && buffer_length > 1)
      snprintf(buffer, buffer_length, "%s", webui_js_empty);
    return false;
  }

  // Initializing pipe
  unsigned char run_id = _webui_get_run_id();
  _webui_core.run_done[run_id] = false;
  _webui_core.run_error[run_id] = false;
  if ((void *)_webui_core.run_responses[run_id] != NULL)
    _webui_free_mem((void *)_webui_core.run_responses[run_id]);

  // Prepare the packet
  size_t packet_len = 3 + js_len; // [header][js]
  char *packet = (char *)_webui_malloc(packet_len);
  packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
  packet[1] = WEBUI_HEADER_JS;        // Type
  packet[2] = run_id;                 // ID
  for (size_t i = 0; i < js_len; i++) // Data
    packet[i + 3] = script[i];

  // Send packets
  _webui_window_send(win, packet, packet_len);
  _webui_free_mem((void *)packet);

  // Wait for UI response
  if (timeout_second < 1 || timeout_second > 86400) {

    // Wait forever
    for (;;) {

      if (_webui_core.run_done[run_id])
        break;

      _webui_sleep(1);
    }
  } else {

    // Using timeout
    for (size_t n = 0; n <= (timeout_second * 1000); n++) {

      if (_webui_core.run_done[run_id])
        break;

      _webui_sleep(1);
    }
  }

  if (_webui_core.run_responses[run_id] != NULL) {

#ifdef WEBUI_LOG
    printf("[User] webui_script -> Response found [%s] \n",
           _webui_core.run_responses[run_id]);
#endif

    // Response found
    if (buffer != NULL && buffer_length > 1) {

      // Copy response to the user's response buffer
      size_t response_len =
          _webui_strlen(_webui_core.run_responses[run_id]) + 1;
      size_t bytes_to_cpy =
          (response_len <= buffer_length ? response_len : buffer_length);
      memcpy(buffer, _webui_core.run_responses[run_id], bytes_to_cpy);
    }

    _webui_free_mem((void *)_webui_core.run_responses[run_id]);

    return _webui_core.run_error[run_id];
  }

  return false;
}

size_t webui_new_window(void) {

#ifdef WEBUI_LOG
  printf("[User] webui_new_window()...\n");
#endif

  _webui_init();

  // Get a new window number
  // starting from 1.
  size_t window_number = webui_get_new_window_id();
  if (_webui_core.wins[window_number] != NULL)
    _webui_panic();

  // Create a new window
  _webui_window_t *win =
      (_webui_window_t *)_webui_malloc(sizeof(_webui_window_t));
  _webui_core.wins[window_number] = win;

  // Initialisation
  win->window_number = window_number;
  win->browser_path = (char *)_webui_malloc(WEBUI_MAX_PATH);
  win->profile_path = (char *)_webui_malloc(WEBUI_MAX_PATH);
  win->server_root_path = (char *)_webui_malloc(WEBUI_MAX_PATH);
  sprintf(win->server_root_path, "%s", WEBUI_DEFAULT_PATH);

#ifdef WEBUI_LOG
  printf("[User] webui_new_window() -> New window #%zu @ 0x%p\n", window_number,
         win);
#endif

  return (size_t)window_number;
}

size_t webui_get_new_window_id(void) {

#ifdef WEBUI_LOG
  printf("[User] webui_get_new_window_id()...\n");
#endif

  for (size_t i = 1; i < WEBUI_MAX_ARRAY; i++) {
    if (_webui_core.wins[i] == NULL) {
      if (i > _webui_core.last_win_number)
        _webui_core.last_win_number = i;
      return i;
    }
  }

  // We should never reach here
  _webui_panic();
  return 0;
}

void webui_new_window_id(size_t window_number) {

#ifdef WEBUI_LOG
  printf("[User] webui_new_window_id([%zu])...\n", window_number);
#endif

  _webui_init();

  if (window_number < 1 || window_number > WEBUI_MAX_ARRAY)
    return;

  // Destroy the window if already exist
  if (_webui_core.wins[window_number] != NULL)
    webui_destroy(window_number);

  // Create a new window
  _webui_window_t *win =
      (_webui_window_t *)_webui_malloc(sizeof(_webui_window_t));
  _webui_core.wins[window_number] = win;

  // Initialisation
  win->window_number = window_number;
  win->browser_path = (char *)_webui_malloc(WEBUI_MAX_PATH);
  win->profile_path = (char *)_webui_malloc(WEBUI_MAX_PATH);
  win->server_root_path = (char *)_webui_malloc(WEBUI_MAX_PATH);
  sprintf(win->server_root_path, "%s", WEBUI_DEFAULT_PATH);

  // Save window ID
  if (window_number > _webui_core.last_win_number)
    _webui_core.last_win_number = window_number;

#ifdef WEBUI_LOG
  printf("[User] webui_new_window_id() -> New window #%zu @ 0x%p\n",
         window_number, win);
#endif
}

void webui_set_kiosk(size_t window, bool status) {

#ifdef WEBUI_LOG
  printf("[User] webui_set_kiosk([%zu])...\n", window);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  win->kiosk_mode = status;
}

void webui_close(size_t window) {

#ifdef WEBUI_LOG
  printf("[User] webui_close([%zu])...\n", window);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  _webui_init();

  if (win->connected) {

    // Prepare packets
    char *packet = (char *)_webui_malloc(4);
    packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
    packet[1] = WEBUI_HEADER_CLOSE;     // Type
    packet[2] = 0;                      // ID
    packet[3] = 0;                      // Data

    // Send packets
    _webui_window_send(win, packet, 4);
    _webui_free_mem((void *)packet);
  }
}

void webui_destroy(size_t window) {

#ifdef WEBUI_LOG
  printf("[User] webui_destroy([%zu])...\n", window);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  _webui_init();

  if (win->server_running) {

    // Freindly close
    webui_close(window);

    // Wait for server threads to stop
    _webui_timer_t timer_1;
    _webui_timer_start(&timer_1);
    for (;;) {
      _webui_sleep(10);
      if (!win->server_running)
        break;
      if (_webui_timer_is_end(&timer_1, 2500))
        break;
    }

    if (win->server_running) {

#ifdef WEBUI_LOG
      printf("[User] webui_destroy([%zu]) -> Forced close...\n", window);
#endif

      // Forced close
      win->connected = false;

      // Wait for server threads to stop
      _webui_timer_t timer_2;
      _webui_timer_start(&timer_2);
      for (;;) {
        _webui_sleep(10);
        if (!win->server_running)
          break;
        if (_webui_timer_is_end(&timer_2, 1500))
          break;
      }
    }
  }

  // Free memory resources
  _webui_free_mem((void *)win->url);
  _webui_free_mem((void *)win->html);
  _webui_free_mem((void *)win->icon);
  _webui_free_mem((void *)win->icon_type);
  _webui_free_mem((void *)win->browser_path);
  _webui_free_mem((void *)win->profile_path);
  _webui_free_mem((void *)win->server_root_path);

  // Free events
  for (size_t i = 1; i < WEBUI_MAX_ARRAY; i++) {
    if (win->event_core[i] != NULL)
      _webui_free_mem((void *)win->event_core[i]);
  }

  // Free window struct
  _webui_free_mem((void *)_webui_core.wins[window]);
  _webui_core.wins[window] = NULL;
}

bool webui_is_shown(size_t window) {

#ifdef WEBUI_LOG
  printf("[User] webui_is_shown([%zu])...\n", window);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return false;
  _webui_window_t *win = _webui_core.wins[window];

  return win->connected;
}

void webui_set_multi_access(size_t window, bool status) {

#ifdef WEBUI_LOG
  printf("[User] webui_set_multi_access([%zu], [%d])...\n", window, status);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  win->multi_access = status;
}

void webui_set_title(size_t window, const char *title) {
#ifdef WEBUI_LOG
  printf("[User] webui_set_title([%zu], [%s])...\n", window, title);
#endif

  // Check window
  if (_webui_core.wins[window] == NULL)
    return;

  // Construct JSON internal callback
  char *internal_callback = _webui_malloc(40 + _webui_strlen(title));
  sprintf(internal_callback,
          "{ \"name\": \"set_title\", \"args\": [ \"%s\" ] }", title);
  size_t len = _webui_strlen(internal_callback);

  // Construct payload
  char *query = (char *)_webui_malloc(3 + len);
  query[0] = WEBUI_HEADER_SIGNATURE;
  query[1] = WEBUI_HEADER_CALL_INTERNAL;
  query[2] = 0;
  for (size_t i = 0; i < len; i++) {
    query[i + 3] = internal_callback[i];
  }

  _webui_window_send(_webui_core.wins[window], query, 3 + len);
  _webui_free_mem((void *)internal_callback);
  _webui_free_mem((void *)query);
}

void webui_set_position(size_t window, unsigned int position_x,
                        unsigned int position_y) {
#ifdef WEBUI_LOG
  printf("[User] webui_set_position([%zu], [%d], [%d])...\n", window,
         position_x, position_y);
#endif

  // Check window
  if (_webui_core.wins[window] == NULL)
    return;

  // Construct JSON internal callback
  char *internal_callback = _webui_malloc(47 + 20);
  sprintf(internal_callback,
          "{ \"name\": \"set_position\", \"args\": [ \"%u\", \"%u\" ] }",
          position_x, position_y);
  size_t len = _webui_strlen(internal_callback);

  // Construct payload
  char *query = (char *)_webui_malloc(3 + len);
  query[0] = WEBUI_HEADER_SIGNATURE;
  query[1] = WEBUI_HEADER_CALL_INTERNAL;
  query[2] = 0;
  for (size_t i = 0; i < len; i++) {
    query[i + 3] = internal_callback[i];
  }

  _webui_window_send(_webui_core.wins[window], query, 3 + len);
  _webui_free_mem((void *)internal_callback);
  _webui_free_mem((void *)query);
}

void webui_set_size(size_t window, unsigned int width, unsigned int height) {
#ifdef WEBUI_LOG
  printf("[User] webui_set_size([%zu], [%d], [%d])...\n", window, width,
         height);
#endif

  // Check window
  if (_webui_core.wins[window] == NULL)
    return;

  // Construct JSON internal callback
  char *internal_callback = _webui_malloc(43 + 20);
  sprintf(internal_callback,
          "{ \"name\": \"set_size\", \"args\": [ \"%u\", \"%u\" ] }", width,
          height);
  size_t len = _webui_strlen(internal_callback);

  // Construct payload
  char *query = (char *)_webui_malloc(3 + len);
  query[0] = WEBUI_HEADER_SIGNATURE;
  query[1] = WEBUI_HEADER_CALL_INTERNAL;
  query[2] = 0;
  for (size_t i = 0; i < len; i++) {
    query[i + 3] = internal_callback[i];
  }

  _webui_window_send(_webui_core.wins[window], query, 3 + len);
  _webui_free_mem((void *)internal_callback);
  _webui_free_mem((void *)query);
}

bool webui_get_title(size_t window, char *buffer, size_t buffer_len) {
#ifdef WEBUI_LOG
  printf("[User] webui_get_title([%zu])...\n", window);
#endif

  // Check window
  if (_webui_core.wins[window] == NULL)
    return false;

  // Construct JSON internal callback
  char *internal_callback = "{ \"name\": \"get_title\" }";
  size_t len = _webui_strlen(internal_callback);

  // Initializing response pipe
  unsigned char run_id = _webui_get_run_id();
  _webui_core.run_done[run_id] = false;
  _webui_core.run_error[run_id] = false;
  if ((void *)_webui_core.run_responses[run_id] != NULL)
    _webui_free_mem((void *)_webui_core.run_responses[run_id]);

  // Construct payload
  char *query = (char *)_webui_malloc(3 + len);
  query[0] = WEBUI_HEADER_SIGNATURE;
  query[1] = WEBUI_HEADER_CALL_INTERNAL;
  query[2] = run_id;
  for (size_t i = 0; i < len; i++) {
    query[i + 3] = internal_callback[i];
  }

  // Send query
  _webui_window_send(_webui_core.wins[window], query, 3 + len);
  _webui_free_mem((void *)query);

  // Wait for UI response
  for (;;) {
    if (_webui_core.run_done[run_id])
      break;

    _webui_sleep(1);
  }

  if (_webui_core.run_responses[run_id] != NULL) {

#ifdef WEBUI_LOG
    printf("[User] webui_get_title -> Response found [%s] \n",
           _webui_core.run_responses[run_id]);
#endif

    // Response found
    if (buffer != NULL && buffer_len > 1) {

      // Copy response to the user's response buffer
      size_t response_len =
          _webui_strlen(_webui_core.run_responses[run_id]) + 1;
      size_t bytes_to_cpy =
          (response_len <= buffer_len ? response_len : buffer_len);
      memcpy(buffer, _webui_core.run_responses[run_id], bytes_to_cpy);
    }

    _webui_free_mem((void *)_webui_core.run_responses[run_id]);

    return _webui_core.run_error[run_id];
  }

  return false;
}

bool webui_get_size(size_t window, unsigned int *width, unsigned int *height) {
#ifdef WEBUI_LOG
  printf("[User] webui_get_size([%zu])...\n", window);
#endif

  // Check window
  if (_webui_core.wins[window] == NULL)
    return false;

  // Construct JSON internal callback
  char *internal_callback = "{ \"name\": \"get_size\" }";
  size_t len = _webui_strlen(internal_callback);

  // Initializing response pipe
  unsigned char run_id = _webui_get_run_id();
  _webui_core.run_done[run_id] = false;
  _webui_core.run_error[run_id] = false;
  if ((void *)_webui_core.run_responses[run_id] != NULL)
    _webui_free_mem((void *)_webui_core.run_responses[run_id]);

  // Construct payload
  char *query = (char *)_webui_malloc(3 + len);
  query[0] = WEBUI_HEADER_SIGNATURE;
  query[1] = WEBUI_HEADER_CALL_INTERNAL;
  query[2] = run_id;
  for (size_t i = 0; i < len; i++) {
    query[i + 3] = internal_callback[i];
  }

  // Send query
  _webui_window_send(_webui_core.wins[window], query, 3 + len);
  _webui_free_mem((void *)query);

  // Wait for UI response
  for (;;) {
    if (_webui_core.run_done[run_id])
      break;

    _webui_sleep(1);
  }

  if (_webui_core.run_responses[run_id] != NULL) {

#ifdef WEBUI_LOG
    printf("[User] webui_get_title -> Response found [%s] \n",
           _webui_core.run_responses[run_id]);
#endif

    // Response found
    if (width != NULL && height != NULL) {

      // Copy response to the user's response buffer
      // Convert 8 bytes buffer to 2 32byte integer
      *width = (unsigned char)_webui_core.run_responses[run_id][3] << 24 |
               (unsigned char)_webui_core.run_responses[run_id][2] << 16 |
               (unsigned char)_webui_core.run_responses[run_id][1] << 8 |
               (unsigned char)_webui_core.run_responses[run_id][0];

      *height = (unsigned char)_webui_core.run_responses[run_id][7] << 24 |
                (unsigned char)_webui_core.run_responses[run_id][6] << 16 |
                (unsigned char)_webui_core.run_responses[run_id][5] << 8 |
                (unsigned char)_webui_core.run_responses[run_id][4];
    }

    _webui_free_mem((void *)_webui_core.run_responses[run_id]);

    return _webui_core.run_error[run_id];
  }

  return false;
}

void webui_set_icon(size_t window, const char *icon, const char *icon_type) {

#ifdef WEBUI_LOG
  printf("[User] webui_set_icon([%zu], [%s], [%s])...\n", window, icon,
         icon_type);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  // Some wrappers does not guarantee `icon` to
  // stay valid, so, let's make our own copy.
  // Icon
  size_t len = _webui_strlen(icon);
  const char *icon_cpy = (const char *)_webui_malloc(len);
  memcpy((char *)icon_cpy, icon, len);
  // Icon Type
  len = _webui_strlen(icon_type);
  const char *icon_type_cpy = (const char *)_webui_malloc(len);
  memcpy((char *)icon_type_cpy, icon_type, len);

  // Clean old sets if any
  if (win->icon != NULL)
    _webui_free_mem((void *)win->icon);
  if (win->icon_type != NULL)
    _webui_free_mem((void *)win->icon_type);

  win->icon = icon_cpy;
  win->icon_type = icon_type_cpy;
}

bool webui_show(size_t window, const char *content) {

#ifdef WEBUI_LOG
  printf("[User] webui_show([%zu])...\n", window);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return false;
  _webui_window_t *win = _webui_core.wins[window];

  // Find the best web browser to use
  size_t browser = _webui_core.current_browser != 0
                       ? _webui_core.current_browser
                       : _webui_find_the_best_browser(win);

  // Show the window
  return _webui_show(win, content, browser);
}

bool webui_show_browser(size_t window, const char *content, size_t browser) {

#ifdef WEBUI_LOG
  printf("[User] webui_show_browser([%zu], [%zu])...\n", window, browser);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return false;
  _webui_window_t *win = _webui_core.wins[window];

  return _webui_show(win, content, browser);
}

size_t webui_bind(size_t window, const char *element,
                  void (*func)(webui_event_t *e)) {

#ifdef WEBUI_LOG
  printf("[User] webui_bind([%zu], [%s], [0x%p])...\n", window, element, func);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return 0;
  _webui_window_t *win = _webui_core.wins[window];

  _webui_init();

  int len = 0;
  if (_webui_is_empty(element))
    win->has_events = true;
  else
    len = _webui_strlen(element);

  // [win num][/][element]
  char *webui_internal_id = _webui_malloc(3 + 1 + len);
  sprintf(webui_internal_id, "%zu/%s", win->window_number, element);

  size_t cb_index = _webui_get_cb_index(webui_internal_id);

  if (cb_index > 0) {

    // Replace a reference
    _webui_core.cb[cb_index] = func;

    _webui_free_mem((void *)webui_internal_id);
  } else {

    // New reference
    cb_index = _webui_set_cb_index(webui_internal_id);

    if (cb_index > 0)
      _webui_core.cb[cb_index] = func;
    else
      _webui_free_mem((void *)webui_internal_id);
  }

  return cb_index;
}

const char *webui_get_string(webui_event_t *e) {

#ifdef WEBUI_LOG
  printf("[User] webui_get_string()...\n");
#endif

  if (e->data != NULL) {
    size_t len = _webui_strlen(e->data);
    if (len > 0 && len <= WEBUI_MAX_BUF)
      return (const char *)e->data;
  }

  return webui_empty_string;
}

long long int webui_get_int(webui_event_t *e) {

#ifdef WEBUI_LOG
  printf("[User] webui_get_int()...\n");
#endif

  char *endptr;

  if (e->data != NULL) {
    size_t len = _webui_strlen(e->data);
    if (len > 0 &&
        len <= 20) // 64-bit max is -9,223,372,036,854,775,808 (20 character)
      return strtoll((const char *)e->data, &endptr, 10);
  }

  return 0;
}

bool webui_get_bool(webui_event_t *e) {

#ifdef WEBUI_LOG
  printf("[User] webui_get_bool()...\n");
#endif

  const char *str = webui_get_string(e);
  if (str[0] == 't' || str[0] == 'T') // true || True
    return true;

  return false;
}

void webui_return_int(webui_event_t *e, long long int n) {

#ifdef WEBUI_LOG
  printf("[User] webui_return_int([%lld])...\n", n);
#endif

  // Dereference
  if (_webui_core.wins[e->window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[e->window];

  // Get buffer
  if (win->event_core[e->event_number] == NULL)
    return;
  char **response = &win->event_core[e->event_number]->response;

  // Free
  if (*response != NULL)
    _webui_free_mem((void *)*response);

  // Int to Str
  // 64-bit max is -9,223,372,036,854,775,808 (20 character)
  char *buf = (char *)_webui_malloc(20);
  sprintf(buf, "%lld", n);

  // Set response
  *response = buf;
}

void webui_return_string(webui_event_t *e, const char *s) {

#ifdef WEBUI_LOG
  printf("[User] webui_return_string([%s])...\n", s);
#endif

  if (_webui_is_empty(s))
    return;

  // Dereference
  if (_webui_core.wins[e->window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[e->window];

  // Get buffer
  if (win->event_core[e->event_number] == NULL)
    return;
  char **response = &win->event_core[e->event_number]->response;

  // Free
  if (*response != NULL)
    _webui_free_mem((void *)*response);

  // Copy Str
  int len = _webui_strlen(s);
  char *buf = (char *)_webui_malloc(len);
  memcpy(buf, s, len);

  // Set response
  *response = buf;
}

void webui_return_bool(webui_event_t *e, bool b) {

#ifdef WEBUI_LOG
  printf("[User] webui_return_bool([%d])...\n", b);
#endif

  // Dereference
  if (_webui_core.wins[e->window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[e->window];

  // Get buffer
  if (win->event_core[e->event_number] == NULL)
    return;
  char **response = &win->event_core[e->event_number]->response;

  // Free
  if (*response != NULL)
    _webui_free_mem((void *)*response);

  // Bool to Str
  int len = 1;
  char *buf = (char *)_webui_malloc(len);
  sprintf(buf, "%d", b);

  // Set response
  *response = buf;
}

char *webui_encode(const char *str) {

#ifdef WEBUI_LOG
  printf("[User] webui_encode()...\n");
#endif

  size_t len = strlen(str);
  if (len < 1)
    return NULL;

#ifdef WEBUI_LOG
  printf("[User] webui_encode() -> Text: [%s]\n", str);
#endif

  size_t buf_len = (((len + 2) / 3) * 4) + 8;
  char *buf = (char *)_webui_malloc(buf_len);

  int ret = mg_base64_encode((const unsigned char *)str, len, buf, &buf_len);

  if (ret > (-1)) {

// Failed
#ifdef WEBUI_LOG
    printf("[User] webui_encode() -> Failed (%d).\n", ret);
#endif
    _webui_free_mem((void *)buf);
    return NULL;
  }

#ifdef WEBUI_LOG
  printf("[User] webui_encode() -> Encoded: [%s]\n", buf);
#endif

  // Success
  return buf;
}

char *webui_decode(const char *str) {

#ifdef WEBUI_LOG
  printf("[User] webui_decode()...\n");
#endif

  size_t len = strlen(str);
  if (len < 1)
    return NULL;

#ifdef WEBUI_LOG
  printf("[User] webui_decode() -> Encoded: [%s]\n", str);
#endif

  size_t buf_len = (((len + 2) / 3) * 4) + 8;
  unsigned char *buf = (unsigned char *)_webui_malloc(buf_len);

  int ret = mg_base64_decode(str, len, buf, &buf_len);

  if (ret > (-1)) {

// Failed
#ifdef WEBUI_LOG
    printf("[User] webui_decode() -> Failed (%d).\n", ret);
#endif
    _webui_free_mem((void *)buf);
    return NULL;
  }

#ifdef WEBUI_LOG
  printf("[User] webui_decode() -> Decoded: [%s]\n", buf);
#endif

  // Success
  return (char *)buf;
}

void webui_free(void *ptr) {

#ifdef WEBUI_LOG
  printf("[User] webui_free([0x%p])...\n", ptr);
#endif

  _webui_free_mem(ptr);
}

void *webui_malloc(size_t size) {

#ifdef WEBUI_LOG
  printf("[User] webui_malloc(%zu bytes)...\n", size);
#endif

  return _webui_malloc(size);
}

void webui_exit(void) {

#ifdef WEBUI_LOG
  printf("[User] webui_exit()...\n");
#endif

#ifndef WEBUI_LOG
  // Close all opened windows
  // by sending `close` command

  // Prepare packets
  char *packet = (char *)_webui_malloc(4);
  packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
  packet[1] = WEBUI_HEADER_CLOSE;     // Type
  packet[2] = 0;                      // ID
  packet[3] = 0;                      // Data
  for (size_t i = 1; i <= _webui_core.last_win_number; i++) {
    if (_webui_core.wins[i] != NULL) {
      if (_webui_core.wins[i]->connected) {
        // Send packet
        _webui_window_send(_webui_core.wins[i], packet, 4);
      }
    }
  }
  _webui_free_mem((void *)packet);
#endif

  // Stop all threads
  _webui_core.exit_now = true;

  // Let's give other threads more time to
  // safely exit and finish their cleaning up.
  _webui_sleep(120);
}

void webui_wait(void) {

#ifdef WEBUI_LOG
  printf("[Loop] webui_wait()...\n");
#endif

  _webui_init();

  if (_webui_core.startup_timeout > 0) {

#ifdef WEBUI_LOG
    printf("[Loop] webui_wait() -> Using timeout %zu second\n",
           _webui_core.startup_timeout);
#endif

    // Wait for browser to start
    _webui_wait_for_startup();

#ifdef WEBUI_LOG
    printf("[Loop] webui_wait() -> Wait for connected UI...\n");
#endif

    while (_webui_core.servers > 0) {

#ifdef WEBUI_LOG
      // printf("[%zu/%zu]", _webui_core.servers, _webui_core.connections);
#endif
      _webui_sleep(50);
    }
  } else {

#ifdef WEBUI_LOG
    printf("[Loop] webui_wait() -> Infinite wait...\n");
#endif

    // Infinite wait
    while (!_webui_core.exit_now)
      _webui_sleep(50);
  }

#ifdef WEBUI_LOG
  printf("[Loop] webui_wait() -> Wait finished.\n");
#endif

  // Final cleaning
  _webui_clean();
}

void webui_set_timeout(size_t second) {

#ifdef WEBUI_LOG
  printf("[User] webui_set_timeout([%zu])...\n", second);
#endif

  _webui_init();

  if (second > WEBUI_MAX_TIMEOUT)
    second = WEBUI_MAX_TIMEOUT;

  _webui_core.startup_timeout = second;
}

void webui_set_runtime(size_t window, size_t runtime) {

#ifdef WEBUI_LOG
  printf("[User] webui_script_runtime([%zu], [%zu])...\n", window, runtime);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  _webui_init();

  if (runtime != Deno && runtime != NodeJS)
    win->runtime = None;
  else
    win->runtime = runtime;
}

bool webui_set_root_folder(size_t window, const char *path) {

#ifdef WEBUI_LOG
  printf("[User] webui_set_root_folder([%zu], [%s])...\n", window, path);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return false;
  _webui_window_t *win = _webui_core.wins[window];

  if (win->server_running || _webui_is_empty(path) ||
      (_webui_strlen(path) > WEBUI_MAX_PATH) ||
      !_webui_folder_exist((char *)path)) {

    sprintf(win->server_root_path, "%s", WEBUI_DEFAULT_PATH);
#ifdef WEBUI_LOG
    printf("[User] webui_set_root_folder() -> Failed.\n");
#endif
    return false;
  }

#ifdef WEBUI_LOG
  printf("[User] webui_set_root_folder() -> Success.\n");
#endif
  sprintf(win->server_root_path, "%s", path);
  return true;
}

// -- Interface's Functions ----------------
static void _webui_interface_bind_handler(webui_event_t *e) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_interface_bind_handler()...\n");
#endif

  // Dereference
  if (_webui_core.wins[e->window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[e->window];

  // Generate WebUI internal id
  char *webui_internal_id = _webui_generate_internal_id(win, e->element);
  size_t cb_index = _webui_get_cb_index(webui_internal_id);

  if (cb_index > 0 && _webui_core.cb_interface[cb_index] != NULL) {

#ifdef WEBUI_LOG
    printf(
        "[Core]\t\t_webui_interface_bind_handler() -> User callback @ 0x%p\n",
        _webui_core.cb_interface[cb_index]);
    printf("[Core]\t\t_webui_interface_bind_handler() -> e->event_type [%zu]\n",
           e->event_type);
    printf("[Core]\t\t_webui_interface_bind_handler() -> e->element [%s]\n",
           e->element);
    printf("[Core]\t\t_webui_interface_bind_handler() -> e->data [%s]\n",
           e->data);
    printf("[Core]\t\t_webui_interface_bind_handler() -> e->event_number %zu\n",
           e->event_number);
#endif

// Call cb
#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_interface_bind_handler() -> Calling user "
           "callback...\n\n");
#endif
    _webui_core.cb_interface[cb_index](e->window, e->event_type, e->element,
                                       e->data, e->event_number);
  }

  // Free
  _webui_free_mem((void *)webui_internal_id);

#ifdef WEBUI_LOG
  // Print cb response
  char *response = NULL;
  if (win->event_core[e->event_number] != NULL)
    response = *(&win->event_core[e->event_number]->response);
  printf("[Core]\t\t_webui_interface_bind_handler() -> user-callback response "
         "[%s]\n",
         response);
#endif
}

size_t webui_interface_bind(size_t window, const char *element,
                            void (*func)(size_t, size_t, char *, char *,
                                         size_t)) {

#ifdef WEBUI_LOG
  printf("[User] webui_interface_bind()...\n");
#endif

  // Bind
  size_t cb_index = webui_bind(window, element, _webui_interface_bind_handler);
  _webui_core.cb_interface[cb_index] = func;
  return cb_index;
}

void webui_interface_set_response(size_t window, size_t event_number,
                                  const char *response) {

#ifdef WEBUI_LOG
  printf("[User] webui_interface_set_response()...\n");
  printf("[User] webui_interface_set_response() -> event_number %zu \n",
         event_number);
  printf("[User] webui_interface_set_response() -> Response [%s] \n", response);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return;
  _webui_window_t *win = _webui_core.wins[window];

  // Get internal response buffer
  if (win->event_core[event_number] != NULL) {

    char **buffer = NULL;
    buffer = &win->event_core[event_number]->response;

    // Set the response
    size_t len = _webui_strlen(response);
    *buffer = (char *)_webui_malloc(len);
    strcpy(*buffer, response);

#ifdef WEBUI_LOG
    printf("[User] webui_interface_set_response() -> Internal buffer [%s] \n",
           *buffer);
#endif
  }
}

bool webui_interface_is_app_running(void) {

#ifdef WEBUI_LOG
  // printf("[User] webui_is_app_running()...\n");
#endif

  static bool app_is_running = true;

  // Stop if already flagged
  if (!app_is_running)
    return false;

  // Initialization
  if (!_webui_core.initialized)
    _webui_init();

  // Get app status
  if (_webui_core.exit_now) {
    app_is_running = false;
  } else if (_webui_core.startup_timeout > 0) {
    if (_webui_core.servers < 1)
      app_is_running = false;
  }

  // Final cleaning
  if (!app_is_running) {
#ifdef WEBUI_LOG
    printf("[User] webui_is_app_running() -> App Stopped.\n");
#endif
    _webui_clean();
  }

  return app_is_running;
}

size_t webui_interface_get_window_id(size_t window) {

#ifdef WEBUI_LOG
  printf("[User] webui_interface_get_window_id()...\n");
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return 0;
  _webui_window_t *win = _webui_core.wins[window];

  return win->window_number;
}

size_t webui_interface_get_bind_id(size_t window, const char *element) {

#ifdef WEBUI_LOG
  printf("[User] webui_interface_get_bind_id([%zu], [%s])...\n", window,
         element);
#endif

  // Dereference
  if (_webui_core.wins[window] == NULL)
    return 0;
  _webui_window_t *win = _webui_core.wins[window];

  size_t len = _webui_strlen(element);
  if (len < 1)
    element = webui_empty_string;

  // [win num][/][element]
  char *webui_internal_id = _webui_malloc(3 + 1 + len);
  sprintf(webui_internal_id, "%zu/%s", win->window_number, element);

  size_t cb_index = _webui_get_cb_index(webui_internal_id);

  _webui_free_mem((void *)webui_internal_id);
  return cb_index;
}

// -- Core's Functions ----------------
static bool _webui_ptr_exist(void *ptr) {

#ifdef WEBUI_LOG
  // printf("[Core]\t\t_webui_ptr_exist()...\n");
#endif

  if (ptr == NULL)
    return false;

  for (size_t i = 0; i < _webui_core.ptr_position; i++) {

    if (_webui_core.ptr_list[i] == ptr)
      return true;
  }

  return false;
}

static void _webui_ptr_add(void *ptr, size_t size) {

#ifdef WEBUI_LOG
  // printf("[Core]\t\t_webui_ptr_add(0x%p)...\n", ptr);
#endif

  if (ptr == NULL)
    return;

  if (!_webui_ptr_exist(ptr)) {

    for (size_t i = 0; i < _webui_core.ptr_position; i++) {

      if (_webui_core.ptr_list[i] == NULL) {

#ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_ptr_add(0x%p) -> Allocate %zu bytes\n", ptr,
               size);
#endif

        _webui_core.ptr_list[i] = ptr;
        _webui_core.ptr_size[i] = size;
        return;
      }
    }

#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_ptr_add(0x%p) -> Allocate %zu bytes\n", ptr, size);
#endif

    _webui_core.ptr_list[_webui_core.ptr_position] = ptr;
    _webui_core.ptr_size[_webui_core.ptr_position] = size;
    _webui_core.ptr_position++;
    if (_webui_core.ptr_position >= WEBUI_MAX_ARRAY)
      _webui_core.ptr_position = (WEBUI_MAX_ARRAY - 1);
  }
}

static void _webui_free_mem(void *ptr) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_free_mem(0x%p)...\n", ptr);
#endif

  if (ptr == NULL)
    return;

  for (size_t i = 0; i < _webui_core.ptr_position; i++) {

    if (_webui_core.ptr_list[i] == ptr) {

#ifdef WEBUI_LOG
      printf("[Core]\t\t_webui_free_mem(0x%p) -> Free %zu bytes\n", ptr,
             _webui_core.ptr_size[i]);
#endif

      memset(ptr, 0, _webui_core.ptr_size[i]);
      free(ptr);

      _webui_core.ptr_size[i] = 0;
      _webui_core.ptr_list[i] = NULL;
    }
  }

  for (int i = _webui_core.ptr_position; i >= 0; i--) {

    if (_webui_core.ptr_list[i] == NULL) {

      _webui_core.ptr_position = i;
      break;
    }
  }
}

static void _webui_free_all_mem(void) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_free_all_mem()...\n");
#endif

  // Makes sure we run this once
  static bool freed = false;
  if (freed)
    return;
  freed = true;

  void *ptr = NULL;
  for (size_t i = 0; i < _webui_core.ptr_position; i++) {

    ptr = _webui_core.ptr_list[i];

    if (ptr != NULL) {

#ifdef WEBUI_LOG
      printf("[Core]\t\t_webui_free_all_mem() -> Free %zu bytes @ 0x%p\n",
             _webui_core.ptr_size[i], ptr);
#endif

      memset(ptr, 0, _webui_core.ptr_size[i]);
      free(ptr);
    }
  }
}

static void _webui_panic(void) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_panic().\n");
#endif

  webui_exit();
  exit(EXIT_FAILURE);
}

static size_t _webui_round_to_memory_block(size_t size) {

  // If size is negative
  if (size < 4)
    size = 4;

  // If size is already a full block
  // we should return the same block
  size--;

  int block_size = 4;
  while (block_size <= size)
    block_size *= 2;

  return (size_t)block_size;
}

static void *_webui_malloc(size_t size) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_malloc([%zu])...\n", size);
#endif

  // Make sure we have the null
  // terminator if it's a string
  size++;

  size = _webui_round_to_memory_block(size);

  void *block = NULL;
  for (size_t i = 0; i < 8; i++) {

    if (size > WEBUI_MAX_BUF)
      size = WEBUI_MAX_BUF;

    block = malloc(size);

    if (block == NULL)
      size++;
    else
      break;
  }

  if (block == NULL) {

    _webui_panic();
    return NULL;
  }

  memset(block, 0, size);

  _webui_ptr_add((void *)block, size);

  return block;
}

static _webui_window_t *_webui_dereference_win_ptr(void *ptr) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_dereference_win_ptr()...\n");
#endif

  _webui_window_t *win = (_webui_window_t *)ptr;

  for (size_t i = 1; i < WEBUI_MAX_ARRAY; i++) {
    if (_webui_core.wins[i] == win)
      return win;
  }

  // This pointer is not a valid
  // webui window struct
  return NULL;
}

static size_t _webui_get_free_event_core_pos(_webui_window_t *win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_free_event_core_pos()...\n");
#endif

  for (size_t i = 1; i < WEBUI_MAX_ARRAY; i++) {
    if (win->event_core[i] == NULL)
      return i;
  }

  // Fatal. No free pos found
  // let's use the first pos
  return 0;
}

static void _webui_sleep(long unsigned int ms) {

#ifdef WEBUI_LOG
  // printf("[Core]\t\t_webui_sleep([%zu])...\n", ms);
#endif

#ifdef _WIN32
  Sleep(ms);
#else
  usleep(ms);
#endif
}

static long _webui_timer_diff(struct timespec *start, struct timespec *end) {

#ifdef WEBUI_LOG
  // printf("[Core]\t\t_webui_timer_diff()...\n");
#endif

  return ((long)(end->tv_sec * 1000) + (long)(end->tv_nsec / 1000000)) -
         ((long)(start->tv_sec * 1000) + (long)(start->tv_nsec / 1000000));
}

static void _webui_timer_clock_gettime(struct timespec *spec) {

#ifdef WEBUI_LOG
  // printf("[Core]\t\t_webui_timer_clock_gettime()...\n");
#endif

#ifdef _WIN32
  __int64 wintime;
  GetSystemTimeAsFileTime((FILETIME *)&wintime);
  wintime -= ((__int64)116444736000000000);
  spec->tv_sec = wintime / ((__int64)10000000);
  spec->tv_nsec = wintime % ((__int64)10000000) * 100;
#else
  clock_gettime(CLOCK_MONOTONIC, spec);
#endif
}

static void _webui_timer_start(_webui_timer_t *t) {

#ifdef WEBUI_LOG
  // printf("[Core]\t\t_webui_timer_start()...\n");
#endif

  _webui_timer_clock_gettime(&t->start);
}

static bool _webui_timer_is_end(_webui_timer_t *t, size_t ms) {

#ifdef WEBUI_LOG
  // printf("[Core]\t\t_webui_timer_is_end()...\n");
#endif

  _webui_timer_clock_gettime(&t->now);

  size_t def = (size_t)_webui_timer_diff(&t->start, &t->now);
  if (def > ms)
    return true;
  return false;
}

static bool _webui_is_empty(const char *s) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_is_empty()...\n");
#endif

  if ((s != NULL) && (s[0] != '\0'))
    return false;
  return true;
}

static size_t _webui_strlen(const char *s) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_strlen()...\n");
#endif

  if (_webui_is_empty(s))
    return 0;

  size_t length = 0;

  while ((s[length] != '\0') && (length < WEBUI_MAX_BUF)) {
    length++;
  }

  return length;
}

static bool _webui_file_exist_mg(_webui_window_t *win,
                                 struct mg_connection *conn) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_file_exist_mg()...\n");
#endif

  char *file;
  char *full_path;

  const struct mg_request_info *ri = mg_get_request_info(conn);
  const char *url = ri->local_uri;
  size_t url_len = _webui_strlen(url);

  // Get file name
  file = (char *)_webui_malloc(url_len);
  const char *p = url;
  p++; // Skip "/"
  sprintf(file, "%.*s", (int)(url_len - 1), p);

  // Get full path
  // [current folder][/][file]
  full_path = (char *)_webui_malloc(_webui_strlen(win->server_root_path) + 1 +
                                    _webui_strlen(file));
  sprintf(full_path, "%s%s%s", win->server_root_path, webui_sep, file);

  bool exist = _webui_file_exist(full_path);

  _webui_free_mem((void *)file);
  _webui_free_mem((void *)full_path);

  return exist;
}

static bool _webui_file_exist(char *file) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_file_exist([%s])...\n", file);
#endif

  if (_webui_is_empty(file))
    return false;

  if (WEBUI_FILE_EXIST(file, 0) == 0)
    return true;
  return false;
}

static const char *_webui_get_extension(const char *f) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_extension()...\n");
#endif

  if (f == NULL)
    return webui_empty_string;

  const char *ext = strrchr(f, '.');

  if (ext == NULL || !ext || ext == f)
    return webui_empty_string;
  return ext + 1;
}

static unsigned char _webui_get_run_id(void) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_run_id()...\n");
#endif

  return ++_webui_core.run_last_id;
}

static bool _webui_socket_test_listen_mg(size_t port_num) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_socket_test_listen_mg([%zu])...\n", port_num);
#endif

  // HTTP Port Test
  char *test_port = (char *)_webui_malloc(16);
  sprintf(test_port, "%zu", port_num);

  // Start HTTP Server
  const char *http_options[] = {"listening_ports", test_port, NULL, NULL};
  struct mg_callbacks http_callbacks;
  struct mg_context *http_ctx;
  memset(&http_callbacks, 0, sizeof(http_callbacks));
  http_ctx = mg_start(&http_callbacks, 0, http_options);

  if (http_ctx == NULL) {

    // Cannot listen
    mg_stop(http_ctx);
    return false;
  }

  // Listening success
  mg_stop(http_ctx);

  return true;
}

static bool _webui_port_is_used(size_t port_num) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_port_is_used([%zu])...\n", port_num);
#endif

#ifdef _WIN32
  // Listener test
  if (!_webui_socket_test_listen_win32(port_num))
    return true; // Port is already used
  return false;  // Port is not in use
#else
  // Listener test MG
  if (!_webui_socket_test_listen_mg(port_num))
    return true; // Port is already used
  return false;  // Port is not in use
#endif
}

static char *_webui_get_file_name_from_url(const char *url) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_file_name_from_url([%s])...\n", url);
#endif

  if (_webui_is_empty(url))
    return NULL;

  // Find the position of "://"
  const char *pos = strstr(url, "://");
  if (pos == NULL) {
    pos = url;
  } else {
    // Move the position after "://"
    pos += 3;
  }

  // Find the position of the first '/'
  pos = strchr(pos, '/');
  if (pos == NULL) {
    // Invalid URL
    return NULL;
  } else {
    // Move the position after "/"
    pos++;
  }

  // Copy the path to a new string
  char *file = strdup(pos);

  // Find the position of the first '?'
  char *question_mark = strchr(file, '?');
  if (question_mark != NULL) {
    // Replace '?' with NULL
    *question_mark = '\0';
  }

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_file_name_from_url() -> File name: [%s]\n",
         file);
#endif

  return file;
}

static char *_webui_get_full_path_from_url(_webui_window_t *win,
                                           const char *url) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_full_path_from_url([%s])...\n", url);
#endif

  // Get file name
  char *file = _webui_get_file_name_from_url(url);

  if (file == NULL)
    return NULL;

  size_t url_len = _webui_strlen(url);

  // Get full path
  // [current folder][/][file]
  char *full_path = (char *)_webui_malloc(_webui_strlen(win->server_root_path) +
                                          1 + _webui_strlen(file));
  sprintf(full_path, "%s%s%s", win->server_root_path, webui_sep, file);

#ifdef _WIN32
  // Replace `/` by `\`
  for (int i = 0; full_path[i] != '\0'; i++) {
    if (full_path[i] == '/') {
      full_path[i] = '\\';
    }
  }
#endif

  // Clean
  _webui_free_mem((void *)file);

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_file_name_from_url() -> Full path: [%s]\n",
         full_path);
#endif

  return full_path;
}

static int _webui_serve_file(_webui_window_t *win, struct mg_connection *conn) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_serve_file()...\n");
#endif

  // Serve a normal text based file

  int http_status_code = 0;

  const struct mg_request_info *ri = mg_get_request_info(conn);
  const char *url = ri->local_uri;

  if (win->files_handler != NULL) {

    // Get file content from the external end-user files handler

    int length = 0;
    const void *data = win->files_handler(url, &length);

    if (data != NULL) {

      // File content found (200)

      if (length == 0)
        length = strlen(data);

      // Send header
      int header_ret = mg_send_http_ok(conn, // 200
                                       mg_get_builtin_mime_type(url), length);

      // Send body
      int body_ret = mg_write(conn, data, length);

      // Safely free resources if end-user allocated
      // using `webui_malloc()`. Otherwise just do nothing.
      webui_free((void *)data);

      http_status_code = 200;
    }

    // If `data == NULL` thats mean the external handler
    // did not find the requested file. So WebUI will try
    // looking for the file locally.
  }

  if (http_status_code != 200) {

    // Using internal files handler

    // Get full path
    char *full_path = _webui_get_full_path_from_url(win, url);

    if (_webui_file_exist(full_path)) {

      // 200 - File exist
      mg_send_file(conn, full_path);
      http_status_code = 200;
    } else {

      // 404 - File not exist
      _webui_http_send_error_page(conn, webui_html_res_not_available, 404);

      http_status_code = 404;
    }

    _webui_free_mem((void *)full_path);
  }

  return http_status_code;
}

static bool _webui_deno_exist(_webui_window_t *win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_deno_exist()...\n");
#endif

  static bool found = false;

  if (found)
    return true;

  if (_webui_cmd_sync(win, "deno --version", false) == 0) {

    found = true;
    return true;
  } else
    return false;
}

static bool _webui_nodejs_exist(_webui_window_t *win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_nodejs_exist()...\n");
#endif

  static bool found = false;

  if (found)
    return true;

  if (_webui_cmd_sync(win, "node -v", false) == 0) {

    found = true;
    return true;
  } else
    return false;
}

static const char *_webui_interpret_command(const char *cmd) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_interpret_command([%s])...\n", cmd);
#endif

  // Run the command with redirection of errors to stdout
  // and return the output.

  // Output buffer
  char *out = NULL;

#ifdef _WIN32
  // Redirect stderr to stdout
  char cmd_with_redirection[512] = {0};
  sprintf(cmd_with_redirection, "cmd.exe /c %s 2>&1", cmd);
  _webui_system_win32_out(cmd_with_redirection, &out, false);
#else
  // Redirect stderr to stdout
  char cmd_with_redirection[512] = {0};
  sprintf(cmd_with_redirection, "%s 2>&1", cmd);

  FILE *pipe = WEBUI_POPEN(cmd_with_redirection, "r");

  if (pipe == NULL)
    return NULL;

  // Read STDOUT
  out = (char *)_webui_malloc(WEBUI_CMD_STDOUT_BUF);
  char *line = (char *)_webui_malloc(1024);
  while (fgets(line, 1024, pipe) != NULL)
    strcat(out, line);
  WEBUI_PCLOSE(pipe);

  // Clean
  _webui_free_mem((void *)line);
#endif

  return (const char *)out;
}

static int _webui_interpret_file(_webui_window_t *win,
                                 struct mg_connection *conn, char *index) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_interpret_file()...\n");
#endif

  // Interpret the file using JavaScript/TypeScript runtimes
  // and send back the output. otherwise, send the file as a normal text based

  int interpret_http_stat = 200;
  char *file = NULL;
  char *full_path = NULL;
  const char *query = NULL;

  const struct mg_request_info *ri = mg_get_request_info(conn);
  const char *url = ri->local_uri;
  size_t url_len = _webui_strlen(url);

  // Get file full path
  if (index != NULL && !_webui_is_empty(index)) {

    // Parse as index file

    file = index;
    full_path = index;
  } else {

    // Parse as other non-index files

    // Get file name
    file = _webui_get_file_name_from_url(url);

    // Get full path
    full_path = _webui_get_full_path_from_url(win, url);
  }

  // Get file extension
  const char *extension = _webui_get_extension(file);

  if (strcmp(extension, "ts") == 0 || strcmp(extension, "js") == 0) {

    // TypeScript / JavaScript

    if (!_webui_file_exist(full_path)) {

      // File not exist - 404
      _webui_http_send_error_page(conn, webui_html_res_not_available, 404);

      _webui_free_mem((void *)file);
      _webui_free_mem((void *)full_path);
      return 404;
    }

    // Get query
    query = ri->query_string;

    if (win->runtime == Deno) {

      // Use Deno
      if (_webui_deno_exist(win)) {

        // Set command
        // [disable coloring][file]
        char *cmd = (char *)_webui_malloc(64 + _webui_strlen(full_path));
#ifdef _WIN32
        sprintf(cmd,
                "Set NO_COLOR=1 & Set DENO_NO_UPDATE_CHECK=1 & deno run "
                "--quiet --allow-all --unstable \"%s\" \"%s\"",
                full_path, query);
#else
        sprintf(cmd,
                "NO_COLOR=1; DENO_NO_UPDATE_CHECK=1; deno run --quiet "
                "--allow-all --unstable \"%s\" \"%s\"",
                full_path, query);
#endif

        // Run command
        const char *out = _webui_interpret_command(cmd);

        if (out != NULL) {

          // Send Deno output
          _webui_http_send(conn, // 200
                           "text/plain", out);
        } else {

          // Deno interpretation failed.
          // Serve as a normal text-based file
          mg_send_file(conn, full_path);
        }

        _webui_free_mem((void *)cmd);
        _webui_free_mem((void *)out);
      } else {

        // Deno not installed

        _webui_http_send_error_page(conn, webui_deno_not_found, 500);

        interpret_http_stat = 500;
      }
    } else if (win->runtime == NodeJS) {

      // Use Nodejs

      if (_webui_nodejs_exist(win)) {

        // Set command
        // [node][file]
        char *cmd = (char *)_webui_malloc(16 + _webui_strlen(full_path));
        sprintf(cmd, "node \"%s\" \"%s\"", full_path, query);

        // Run command
        const char *out = _webui_interpret_command(cmd);

        if (out != NULL) {

          // Send Node.js output
          _webui_http_send(conn, // 200
                           "text/plain", out);
        } else {

          // Node.js interpretation failed.
          // Serve as a normal text-based file
          mg_send_file(conn, full_path);
        }

        _webui_free_mem((void *)cmd);
        _webui_free_mem((void *)out);
      } else {

        // Node.js not installed
        _webui_http_send_error_page(conn, webui_nodejs_not_found, 500);

        interpret_http_stat = 500;
      }
    } else {

      // Unknown runtime
      // Serve as a normal text-based file
      mg_send_file(conn, full_path);
    }
  } else {

    // Unknown file extension

    // Serve as a normal text-based file
    interpret_http_stat = _webui_serve_file(win, conn);
  }

  _webui_free_mem((void *)file);
  _webui_free_mem((void *)full_path);

  return interpret_http_stat;
}

static const char *_webui_generate_js_bridge(_webui_window_t *win) {
  // Calculate the cb size
  size_t cb_mem_size =
      64; // To hold 'const _webui_bind_list = ["elem1", "elem2",];'
  for (size_t i = 1; i < WEBUI_MAX_ARRAY; i++)
    if (_webui_core.html_elements[i] != NULL &&
        !_webui_is_empty(_webui_core.html_elements[i]))
      cb_mem_size += _webui_strlen(_webui_core.html_elements[i]) + 3;

  // Generate the cb array
  char *event_cb_js_array = (char *)_webui_malloc(cb_mem_size);
  strcat(event_cb_js_array, "[");
  for (size_t i = 1; i < WEBUI_MAX_ARRAY; i++) {
    if (_webui_core.html_elements[i] != NULL &&
        !_webui_is_empty(_webui_core.html_elements[i])) {
      strcat(event_cb_js_array, "\"");
      strcat(event_cb_js_array, _webui_core.html_elements[i]);
      strcat(event_cb_js_array, "\",");
    }
  }
  strcat(event_cb_js_array, "]");

  // Generate the full WebUI JS-Bridge
  size_t len = cb_mem_size + _webui_strlen(webui_javascript_bridge);
  char *js = (char *)_webui_malloc(len);
#ifdef WEBUI_LOG
  sprintf(js,
          "%s\nglobalThis.webui = new WebUiClient({ port: %zu, winNum: %zu, "
          "bindList: %s, log: true });",
          webui_javascript_bridge, win->ws_port, win->window_number,
          event_cb_js_array);
  printf("[Core]\t\t_webui_generate_js_bridge()...\n");
#else
  sprintf(js,
          "%s\nglobalThis.webui = new WebUiClient({ port: %zu, winNum: %zu, "
          "bindList: %s, log: false });",
          webui_javascript_bridge, win->ws_port, win->window_number,
          event_cb_js_array);
#endif

  return js;
}

static bool _webui_browser_create_profile_folder(_webui_window_t *win,
                                                 size_t browser) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_create_profile_folder(%zu)...\n", browser);
#endif

  const char *temp = _webui_browser_get_temp_path(browser);

  if (browser == Chrome) {

    // Google Chrome
    sprintf(win->profile_path, "%s%s.WebUI%sWebUIChromeProfile", temp,
            webui_sep, webui_sep);
    return true;
  } else if (browser == Edge) {

    // Edge
    sprintf(win->profile_path, "%s%s.WebUI%sWebUIEdgeProfile", temp, webui_sep,
            webui_sep);
    return true;
  } else if (browser == Epic) {

    // Epic
    sprintf(win->profile_path, "%s%s.WebUI%sWebUIEpicProfile", temp, webui_sep,
            webui_sep);
    return true;
  } else if (browser == Vivaldi) {

    // Vivaldi
    sprintf(win->profile_path, "%s%s.WebUI%sWebUIVivaldiProfile", temp,
            webui_sep, webui_sep);
    return true;
  } else if (browser == Brave) {

    // Brave
    sprintf(win->profile_path, "%s%s.WebUI%sWebUIBraveProfile", temp, webui_sep,
            webui_sep);
    return true;
  } else if (browser == Yandex) {

    // Yandex
    sprintf(win->profile_path, "%s%s.WebUI%sWebUIYandexProfile", temp,
            webui_sep, webui_sep);
    return true;
  } else if (browser == Chromium) {

    // Chromium
    sprintf(win->profile_path, "%s%s.WebUI%sWebUIChromiumProfile", temp,
            webui_sep, webui_sep);
    return true;
  } else if (browser == Firefox) {

    // Firefox (We need to create a folder)

    char *profile_name = "WebUIFirefoxProfile";

    char firefox_profile_path[1024] = {0};
    sprintf(firefox_profile_path, "%s%s.WebUI%s%s", temp, webui_sep, webui_sep,
            profile_name);

    if (!_webui_folder_exist(firefox_profile_path)) {

      char buf[2048] = {0};

      sprintf(buf, "%s -CreateProfile \"WebUI %s\"", win->browser_path,
              firefox_profile_path);
      _webui_cmd_sync(win, buf, false);

      // Creating the browser profile
      for (size_t n = 0; n <= 12; n++) {
        // 3000ms
        if (_webui_folder_exist(firefox_profile_path))
          break;
        _webui_sleep(250);
      }

      if (!_webui_folder_exist(firefox_profile_path))
        return false;

      // prefs.js
      FILE *file;
      sprintf(buf, "%s%sprefs.js", firefox_profile_path, webui_sep);
      file = fopen(buf, "a");
      if (file == NULL)
        return false;
      fputs("user_pref(\"toolkit.legacyUserProfileCustomizations.stylesheets\","
            " true); ",
            file);
      fputs("user_pref(\"browser.shell.checkDefaultBrowser\", false); ", file);
      fputs("user_pref(\"browser.tabs.warnOnClose\", false); ", file);
      fclose(file);

      // userChrome.css
      sprintf(buf, "\"%s%schrome%s\"", firefox_profile_path, webui_sep,
              webui_sep);
      if (!_webui_folder_exist(buf)) {

        sprintf(buf, "mkdir \"%s%schrome%s\"", firefox_profile_path, webui_sep,
                webui_sep);
        _webui_cmd_sync(win, buf, false); // Create directory
      }
      sprintf(buf, "%s%schrome%suserChrome.css", firefox_profile_path,
              webui_sep, webui_sep);
      file = fopen(buf, "a");
      if (file == NULL)
        return false;
#ifdef _WIN32
      fputs(
          ":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) "
          "{--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!"
          "important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - "
          "var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!"
          "important;height:var(--uc-toolbar-height)!important;position:"
          "relative}#main-menubar{-moz-box-flex:1;background-color:var(--"
          "toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-"
          "box;border-right:30px solid "
          "transparent;border-image:linear-gradient(to "
          "left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) "
          "30px) 20 / 30px}#toolbar-menubar:not([inactive]) "
          "{z-index:2}#toolbar-menubar[inactive] > "
          "#menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-"
          "window-drag-space-width,0px)}#nav-bar{visibility:collapse}@-moz-"
          "document url(chrome://browser/content/browser.xhtml) "
          "{:root:not([sizemode=\"fullscreen\"]) > head{display: "
          "block;position: fixed;width: calc(200vw - 440px);text-align: "
          "left;z-index: 9;pointer-events: none;}head > *{ display: none }head "
          "> title{display: -moz-inline-box;padding: 4px;max-width: "
          "50vw;overflow-x: hidden;text-overflow: ellipsis;}}",
          file);
#elif __APPLE__
      fputs(
          ":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) "
          "{--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!"
          "important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - "
          "var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!"
          "important;height:var(--uc-toolbar-height)!important;position:"
          "relative}#main-menubar{-moz-box-flex:1;background-color:var(--"
          "toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-"
          "box;border-right:30px solid "
          "transparent;border-image:linear-gradient(to "
          "left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) "
          "30px) 20 / 30px}#toolbar-menubar:not([inactive]) "
          "{z-index:2}#toolbar-menubar[inactive] > "
          "#menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-"
          "window-drag-space-width,0px)}#nav-bar{visibility:collapse}@-moz-"
          "document url(chrome://browser/content/browser.xhtml) "
          "{:root:not([sizemode=\"fullscreen\"]) > head{display: "
          "block;position: fixed;width: calc(200vw - 440px);text-align: "
          "left;z-index: 9;pointer-events: none;}head > *{ display: none }head "
          "> title{display: -moz-inline-box;padding: 4px;max-width: "
          "50vw;overflow-x: hidden;text-overflow: ellipsis;}}",
          file);
#else
      fputs(
          ":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) "
          "{--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!"
          "important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - "
          "var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!"
          "important;height:var(--uc-toolbar-height)!important;position:"
          "relative}#main-menubar{-moz-box-flex:1;background-color:var(--"
          "toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-"
          "box;border-right:30px solid "
          "transparent;border-image:linear-gradient(to "
          "left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) "
          "30px) 20 / 30px}#toolbar-menubar:not([inactive]) "
          "{z-index:2}#toolbar-menubar[inactive] > "
          "#menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-"
          "window-drag-space-width,0px)}#nav-bar{visibility:collapse}@-moz-"
          "document url(chrome://browser/content/browser.xhtml) "
          "{:root:not([sizemode=\"fullscreen\"]) > head{display: "
          "block;position: fixed;width: calc(200vw - 440px);text-align: "
          "left;z-index: 9;pointer-events: none;}head > *{ display: none }head "
          "> title{display: -moz-inline-box;padding: 4px;max-width: "
          "50vw;overflow-x: hidden;text-overflow: ellipsis;}}",
          file);
#endif
      fclose(file);

      sprintf(win->profile_path, "%s%s%s", temp, webui_sep, profile_name);
    }

    return true;
  }

  return false;
}

static bool _webui_folder_exist(char *folder) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_folder_exist([%s])...\n", folder);
#endif

#if defined(_MSC_VER)
  if (GetFileAttributesA(folder) != INVALID_FILE_ATTRIBUTES)
    return true;
#else
  DIR *dir = opendir(folder);
  if (dir) {
    closedir(dir);
    return true;
  }
#endif

  return false;
}

static char *_webui_generate_internal_id(_webui_window_t *win,
                                         const char *element) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_generate_internal_id([%s])...\n", element);
#endif

  // Generate WebUI internal id
  size_t element_len = _webui_strlen(element);
  size_t internal_id_size = 3 + 1 + element_len; // [win num][/][name]
  char *webui_internal_id = (char *)_webui_malloc(internal_id_size);
  sprintf(webui_internal_id, "%zu/%s", win->window_number, element);

  return webui_internal_id;
}

static const char *_webui_browser_get_temp_path(size_t browser) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_get_temp_path([%zu])...\n", browser);
#endif

#ifdef _WIN32
// Resolve %USERPROFILE%
#ifdef _MSC_VER
  char *WinUserProfile = NULL;
  size_t sz = 0;
  if (_dupenv_s(&WinUserProfile, &sz, "USERPROFILE") != 0 ||
      WinUserProfile == NULL)
    return webui_empty_string;
#else
  char *WinUserProfile = getenv("USERPROFILE");
  if (WinUserProfile == NULL)
    return webui_empty_string;
#endif
#elif __APPLE__
  // Resolve $HOME
  char *MacUserProfile = getenv("HOME");
  if (MacUserProfile == NULL)
    return webui_empty_string;
#else
  // Resolve $HOME
  char *LinuxUserProfile = getenv("HOME");
  if (LinuxUserProfile == NULL)
    return webui_empty_string;
#endif

#ifdef _WIN32
  return WinUserProfile;
#elif __APPLE__
  return MacUserProfile;
#else
  return LinuxUserProfile;
#endif
}

static bool _webui_is_google_chrome_folder(const char *folder) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_is_google_chrome_folder([%s])...\n", folder);
#endif

  char browser_full_path[WEBUI_MAX_PATH];

  // Make sure this folder is Google Chrome setup and not Chromium
  // by checking if `master_preferences` file exist or `initial_preferences`
  // Ref: https://support.google.com/chrome/a/answer/187948?hl=en

  sprintf(browser_full_path, "%s\\master_preferences", folder);
  if (!_webui_file_exist(browser_full_path)) {

    sprintf(browser_full_path, "%s\\initial_preferences", folder);
    if (!_webui_file_exist(browser_full_path))
      return false; // This is Chromium or something else
  }

  // Make sure the browser executable file exist
  sprintf(browser_full_path, "%s\\chrome.exe", folder);
  if (!_webui_file_exist(browser_full_path))
    return false;

  return true;
}

static bool _webui_browser_exist(_webui_window_t *win, size_t browser) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_exist([%zu])...\n", browser);
#endif

  // Check if a web browser is installed on this machine

  if (browser == Chrome) {

    // Google Chrome

    static bool ChromeExist = false;
    if (ChromeExist && !_webui_is_empty(win->browser_path))
      return true;

#ifdef _WIN32

    // Google Chrome on Windows

    char browser_folder[WEBUI_MAX_PATH];

    // Search in `HKEY_LOCAL_MACHINE` (If Google Chrome installed for
    // multi-user)
    if (_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\chrome.exe",
                                     L"Path", browser_folder)) {

      // Make sure its Google Chrome and not Chromium
      if (_webui_is_google_chrome_folder(browser_folder)) {

        // Google Chrome Found (multi-user)
        sprintf(win->browser_path, "\"%s\\chrome.exe\"", browser_folder);
        ChromeExist = true;
        return true;
      }
    }

    // Search in `HKEY_CURRENT_USER` (If Google Chrome installed for one user)
    if (_webui_get_windows_reg_value(HKEY_CURRENT_USER,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\chrome.exe",
                                     L"Path", browser_folder)) {

      // Make sure its Google Chrome and not Chromium
      if (_webui_is_google_chrome_folder(browser_folder)) {

        // Google Chrome Found (one user)
        sprintf(win->browser_path, "\"%s\\chrome.exe\"", browser_folder);
        ChromeExist = true;
        return true;
      }
    }

    return false;

#elif __APPLE__

    // Google Chrome on macOS
    if (_webui_cmd_sync(win, "open -R -a \"Google Chrome\"", false) == 0) {

      sprintf(win->browser_path, "open --new -a \"Google Chrome.app\" --args");
      ChromeExist = true;
      return true;
    } else
      return false;
#else

    // Google Chrome on Linux
    if (_webui_cmd_sync(win, "google-chrome --version", false) == 0) {

      sprintf(win->browser_path, "google-chrome");
      ChromeExist = true;
      return true;
    } else if (_webui_cmd_sync(win, "google-chrome-stable --version", false) ==
               0) {

      sprintf(win->browser_path, "google-chrome-stable");
      ChromeExist = true;
      return true;
    } else
      return false;

#endif
  } else if (browser == Edge) {

    // Edge

    static bool EdgeExist = false;
    if (EdgeExist && !_webui_is_empty(win->browser_path))
      return true;

#ifdef _WIN32

    // Edge on Windows

    char browser_fullpath[WEBUI_MAX_PATH];

    // Search in `HKEY_LOCAL_MACHINE` (If Edge installed for multi-user)
    if (_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\msedge.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Edge Found (multi-user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        EdgeExist = true;
        return true;
      }
    }

    // Search in `HKEY_CURRENT_USER` (If Edge installed for one user)
    if (_webui_get_windows_reg_value(HKEY_CURRENT_USER,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\msedge.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Edge Found (one user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        EdgeExist = true;
        return true;
      }
    }

    return false;

#elif __APPLE__

    // Edge on macOS
    if (_webui_cmd_sync(win, "open -R -a \"Microsoft Edge\"", false) == 0) {

      sprintf(win->browser_path, "open --new -a \"Microsoft Edge.app\" --args");
      EdgeExist = true;
      return true;
    } else
      return false;

#else

    // Edge on Linux
    if (_webui_cmd_sync(win, "microsoft-edge-stable --version", false) == 0) {

      sprintf(win->browser_path, "microsoft-edge-stable");
      EdgeExist = true;
      return true;
    } else if (_webui_cmd_sync(win, "microsoft-edge-dev --version", false) ==
               0) {

      sprintf(win->browser_path, "microsoft-edge-dev");
      EdgeExist = true;
      return true;
    } else
      return false;

#endif
  } else if (browser == Epic) {

    // Epic Privacy Browser

    static bool EpicExist = false;
    if (EpicExist && !_webui_is_empty(win->browser_path))
      return true;

#ifdef _WIN32

    // Epic on Windows

    char browser_fullpath[WEBUI_MAX_PATH];

    // Search in `HKEY_CURRENT_USER` (If Epic installed for one user)
    if (_webui_get_windows_reg_value(HKEY_CURRENT_USER,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\epic.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Epic Found (one user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        EpicExist = true;
        return true;
      }
    }

    // Search in `HKEY_LOCAL_MACHINE` (If Epic installed for multi-user)
    if (_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\epic.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Epic Found (multi-user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        EpicExist = true;
        return true;
      }
    }

    return false;

#elif __APPLE__

    // Epic on macOS
    if (_webui_cmd_sync(win, "open -R -a \"Epic\"", false) == 0) {

      sprintf(win->browser_path, "open --new -a \"Epic.app\" --args");
      EpicExist = true;
      return true;
    } else
      return false;
#else

    // Epic on Linux
    if (_webui_cmd_sync(win, "epic --version", false) == 0) {

      sprintf(win->browser_path, "epic");
      EpicExist = true;
      return true;
    } else
      return false;
#endif
  } else if (browser == Vivaldi) {

    // Vivaldi Browser

    static bool VivaldiExist = false;
    if (VivaldiExist && !_webui_is_empty(win->browser_path))
      return true;

#ifdef _WIN32

    // Vivaldi on Windows

    char browser_fullpath[WEBUI_MAX_PATH];

    // Search in `HKEY_LOCAL_MACHINE` (If Vivaldi installed for multi-user)
    if (_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\vivaldi.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Vivaldi Found (multi-user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        VivaldiExist = true;
        return true;
      }
    }

    // Search in `HKEY_CURRENT_USER` (If Vivaldi installed for one user)
    if (_webui_get_windows_reg_value(HKEY_CURRENT_USER,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\vivaldi.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Vivaldi Found (one user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        VivaldiExist = true;
        return true;
      }
    }

    return false;

#elif __APPLE__

    // Vivaldi on macOS
    if (_webui_cmd_sync(win, "open -R -a \"Vivaldi\"", false) == 0) {

      sprintf(win->browser_path, "open --new -a \"Vivaldi.app\" --args");
      VivaldiExist = true;
      return true;
    } else
      return false;
#else

    // Vivaldi on Linux
    if (_webui_cmd_sync(win, "vivaldi --version", false) == 0) {

      sprintf(win->browser_path, "vivaldi");
      VivaldiExist = true;
      return true;
    } else if (_webui_cmd_sync(win, "vivaldi-stable --version", false) == 0) {

      sprintf(win->browser_path, "vivaldi-stable");
      VivaldiExist = true;
      return true;
    } else if (_webui_cmd_sync(win, "vivaldi-snapshot --version", false) == 0) {

      sprintf(win->browser_path, "vivaldi-snapshot");
      VivaldiExist = true;
      return true;
    } else
      return false;
#endif
  } else if (browser == Brave) {

    // Brave Browser

    static bool BraveExist = false;
    if (BraveExist && !_webui_is_empty(win->browser_path))
      return true;

#ifdef _WIN32

    // Brave on Windows

    char browser_fullpath[WEBUI_MAX_PATH];

    // Search in `HKEY_LOCAL_MACHINE` (If Brave installed for multi-user)
    if (_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\brave.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Brave Found (multi-user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        BraveExist = true;
        return true;
      }
    }

    // Search in `HKEY_CURRENT_USER` (If Brave installed for one user)
    if (_webui_get_windows_reg_value(HKEY_CURRENT_USER,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\brave.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Brave Found (one user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        BraveExist = true;
        return true;
      }
    }

    return false;

#elif __APPLE__

    // Brave on macOS
    if (_webui_cmd_sync(win, "open -R -a \"Brave Browser\"", false) == 0) {

      sprintf(win->browser_path, "open --new -a \"Brave Browser.app\" --args");
      BraveExist = true;
      return true;
    } else
      return false;
#else

    // Brave on Linux
    if (_webui_cmd_sync(win, "brave --version", false) == 0) {

      sprintf(win->browser_path, "brave");
      BraveExist = true;
      return true;
    } else
      return false;
#endif
  } else if (browser == Firefox) {

    // Firefox

    static bool FirefoxExist = false;
    if (FirefoxExist && !_webui_is_empty(win->browser_path))
      return true;

#ifdef _WIN32

    // Firefox on Windows

    char browser_fullpath[WEBUI_MAX_PATH];

    // Search in `HKEY_LOCAL_MACHINE` (If Firefox installed for multi-user)
    if (_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\firefox.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Firefox Found (multi-user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        FirefoxExist = true;
        return true;
      }
    }

    // Search in `HKEY_CURRENT_USER` (If Firefox installed for one user)
    if (_webui_get_windows_reg_value(HKEY_CURRENT_USER,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\firefox.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Firefox Found (one user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        FirefoxExist = true;
        return true;
      }
    }

    return false;

#elif __APPLE__

    // Firefox on macOS
    if (_webui_cmd_sync(win, "open -R -a \"Firefox\"", false) == 0) {

      sprintf(win->browser_path, "open --new -a \"Firefox.app\" --args");
      FirefoxExist = true;
      return true;
    } else
      return false;
#else

    // Firefox on Linux

    if (_webui_cmd_sync(win, "firefox -v", false) == 0) {

      sprintf(win->browser_path, "firefox");
      FirefoxExist = true;
      return true;
    } else
      return false;

#endif

  } else if (browser == Yandex) {

    // Yandex Browser

    static bool YandexExist = false;
    if (YandexExist && !_webui_is_empty(win->browser_path))
      return true;

#ifdef _WIN32

    // Yandex on Windows

    char browser_fullpath[WEBUI_MAX_PATH];

    // Search in `HKEY_CURRENT_USER` (If Yandex installed for one user)
    if (_webui_get_windows_reg_value(HKEY_CURRENT_USER,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\browser.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Yandex Found (one user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        YandexExist = true;
        return true;
      }
    }

    // Search in `HKEY_LOCAL_MACHINE` (If Yandex installed for multi-user)
    if (_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\browser.exe",
                                     L"", browser_fullpath)) {

      // Make sure the browser executable file exist
      if (_webui_file_exist(browser_fullpath)) {

        // Yandex Found (multi-user)
        sprintf(win->browser_path, "\"%s\"", browser_fullpath);
        YandexExist = true;
        return true;
      }
    }

    return false;

#elif __APPLE__

    // Yandex on macOS
    if (_webui_cmd_sync(win, "open -R -a \"Yandex\"", false) == 0) {

      sprintf(win->browser_path, "open --new -a \"Yandex.app\" --args");
      YandexExist = true;
      return true;
    } else
      return false;
#else

    // Yandex on Linux
    if (_webui_cmd_sync(win, "yandex-browser --version", false) == 0) {

      sprintf(win->browser_path, "yandex-browser");
      YandexExist = true;
      return true;
    } else
      return false;
#endif
  } else if (browser == Chromium) {

    // The Chromium Projects

    static bool ChromiumExist = false;
    if (ChromiumExist && !_webui_is_empty(win->browser_path))
      return true;

#ifdef _WIN32

    // Chromium on Windows

    char browser_folder[WEBUI_MAX_PATH];

    // Search in `HKEY_CURRENT_USER` (If Chromium installed for one user)
    if (_webui_get_windows_reg_value(HKEY_CURRENT_USER,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\chrome.exe",
                                     L"Path", browser_folder)) {

      // Make sure its Chromium and not Google Chrome
      if (!_webui_is_google_chrome_folder(browser_folder)) {

        // Chromium Found (one user)
        sprintf(win->browser_path, "\"%s\\chrome.exe\"", browser_folder);
        ChromiumExist = true;
        return true;
      }
    }

    // Search in `HKEY_LOCAL_MACHINE` (If Chromium installed for multi-user)
    if (_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE,
                                     L"Software\\Microsoft\\Windows\\CurrentVer"
                                     L"sion\\App Paths\\chrome.exe",
                                     L"Path", browser_folder)) {

      // Make sure its Chromium and not Google Chrome
      if (!_webui_is_google_chrome_folder(browser_folder)) {

        // Chromium Found (multi-user)
        sprintf(win->browser_path, "\"%s\\chrome.exe\"", browser_folder);
        ChromiumExist = true;
        return true;
      }
    }

    return false;

#elif __APPLE__

    // Chromium on macOS
    if (_webui_cmd_sync(win, "open -R -a \"Chromium\"", false) == 0) {

      sprintf(win->browser_path, "open --new -a \"Chromium.app\" --args");
      ChromiumExist = true;
      return true;
    } else
      return false;
#else

    // Chromium on Linux
    if (_webui_cmd_sync(win, "chromium-browser --version", false) == 0) {

      sprintf(win->browser_path, "chromium-browser");
      ChromiumExist = true;
      return true;
    } else if (_webui_cmd_sync(win, "chromium --version", false) == 0) {

      sprintf(win->browser_path, "chromium");
      ChromiumExist = true;
      return true;
    } else
      return false;
#endif
  }

  return false;
}

static void _webui_clean(void) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_clean()...\n");
#endif

  static bool cleaned = false;
  if (cleaned)
    return;
  cleaned = true;

  // Let's give other threads more time to safely exit
  // and finish their cleaning up.
  _webui_sleep(250);

  // TODO: Add option to let the user decide if
  // WebUI should delete the web browser profile
  // folder or not.

  // Clean all servers services
  mg_exit_library();

  // Free all non-freed memory allocations
  _webui_free_all_mem();
}

static int _webui_cmd_sync(_webui_window_t *win, char *cmd, bool show) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_cmd_sync()...\n");
#endif

  // Run sync command and
  // return the exit code

  char buf[2048] = {0};

// Sync command
#ifdef _WIN32
  // Using: _CMD_
  sprintf(buf, "cmd /c \"%s\" > nul 2>&1", cmd);
#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_cmd_sync() -> Running [%s] \n", buf);
#endif
  return _webui_system_win32(win, buf, show);
#else
  // Using: _CMD_
  sprintf(buf, "%s >>/dev/null 2>>/dev/null ", cmd);
#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_cmd_sync() -> Running [%s] \n", buf);
#endif
  int r = system(buf);
  r = (r != -1 && r != 127 && WIFEXITED(r)) ? WEXITSTATUS(r) : -1;
  return r;
#endif
}

static int _webui_cmd_async(_webui_window_t *win, char *cmd, bool show) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_cmd_async()...\n");
#endif

  // Run a async command
  // and return immediately

  char buf[1024] = {0};

// Asynchronous command
#ifdef _WIN32
  // Using: START "" _CMD_
  sprintf(buf, "cmd /c \"START \"\" %s\" > nul 2>&1", cmd);
#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_cmd_async() -> Running [%s] \n", buf);
#endif
  return _webui_system_win32(win, buf, show);
#else
  // Using: _CMD_ &
  sprintf(buf, "%s >>/dev/null 2>>/dev/null &", cmd);
#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_cmd_async() -> Running [%s] \n", buf);
#endif
  int r = system(buf);
  r = (r != -1 && r != 127 && WIFEXITED(r)) ? WEXITSTATUS(r) : -1;
  return r;
#endif
}

static int _webui_run_browser(_webui_window_t *win, char *cmd) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_run_browser()...\n");
#endif

  // Run a async command
  return _webui_cmd_async(win, cmd, false);
}

static int _webui_get_browser_args(_webui_window_t *win, size_t browser,
                                   char *buffer, size_t max) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_browser_args([%zu])...\n", browser);
#endif

  const char *chromium_options[] = {
      "--no-first-run",
      "--disable-gpu",
      "--disable-software-rasterizer",
      "--no-proxy-server",
      "--safe-mode",
      "--disable-extensions",
      "--disable-background-mode",
      "--disable-plugins",
      "--disable-plugins-discovery",
      "--disable-translate",
      "--bwsi",
      "--disable-sync",
      "--disable-sync-preferences",
  };

  int c = 0;
  switch (browser) {
  case Chrome:
  case Edge:
  case Epic:
  case Vivaldi:
  case Brave:
  case Yandex:
  case Chromium:
    c = sprintf(buffer, " --user-data-dir=\"%s\"", win->profile_path);
    for (int i = 0;
         i < (int)(sizeof(chromium_options) / sizeof(chromium_options[0]));
         i++) {
      c += sprintf(buffer + c, " %s", chromium_options[i]);
    }

    if (win->kiosk_mode)
      c += sprintf(buffer + c, " %s", "--chrome-frame --kiosk");

    c += sprintf(buffer + c, " %s", "--app=");
    return c;
  case Firefox:
    c = sprintf(buffer, " -P WebUI -purgecaches");
    if (win->kiosk_mode)
      c += sprintf(buffer, "%s", " -kiosk");
    c += sprintf(buffer, " -new-window ");
    return c;
  }

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_browser_args() -> Unknown Browser (%zu)\n",
         browser);
#endif
  strcpy(buffer, "");
  return 0;
}

static bool _webui_browser_start_chrome(_webui_window_t *win,
                                        const char *address) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start_chrome([%s])...\n", address);
#endif

  // -- Google Chrome ----------------------

  if (win->current_browser != 0 && win->current_browser != Chrome)
    return false;

  if (!_webui_browser_exist(win, Chrome))
    return false;

  if (!_webui_browser_create_profile_folder(win, Chrome))
    return false;

  char arg[1024] = {0};
  _webui_get_browser_args(win, Chrome, arg, sizeof(arg));

  char full[1024] = {0};
  sprintf(full, "%s%s%s", win->browser_path, arg, address);

  if (_webui_run_browser(win, full) == 0) {

    win->current_browser = Chrome;
    _webui_core.current_browser = Chrome;
    return true;
  } else
    return false;
}

static bool _webui_browser_start_edge(_webui_window_t *win,
                                      const char *address) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start_edge([%s])...\n", address);
#endif

  // -- Microsoft Edge ----------------------

  if (win->current_browser != 0 && win->current_browser != Edge)
    return false;

  if (!_webui_browser_exist(win, Edge))
    return false;

  if (!_webui_browser_create_profile_folder(win, Edge))
    return false;

  char arg[1024] = {0};
  _webui_get_browser_args(win, Edge, arg, sizeof(arg));

  char full[1024] = {0};
  sprintf(full, "%s%s%s", win->browser_path, arg, address);

  if (_webui_run_browser(win, full) == 0) {

    win->current_browser = Edge;
    _webui_core.current_browser = Edge;
    return true;
  } else
    return false;
}

static bool _webui_browser_start_epic(_webui_window_t *win,
                                      const char *address) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start_epic([%s])...\n", address);
#endif

  // -- Epic Privacy Browser ----------------------

  if (win->current_browser != 0 && win->current_browser != Epic)
    return false;

  if (!_webui_browser_exist(win, Epic))
    return false;

  if (!_webui_browser_create_profile_folder(win, Epic))
    return false;

  char arg[1024] = {0};
  _webui_get_browser_args(win, Epic, arg, sizeof(arg));

  char full[1024] = {0};
  sprintf(full, "%s%s%s", win->browser_path, arg, address);

  if (_webui_run_browser(win, full) == 0) {

    win->current_browser = Epic;
    _webui_core.current_browser = Epic;
    return true;
  } else
    return false;
}

static bool _webui_browser_start_vivaldi(_webui_window_t *win,
                                         const char *address) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start_vivaldi([%s])...\n", address);
#endif

  // -- Vivaldi Browser ----------------------

  if (win->current_browser != 0 && win->current_browser != Vivaldi)
    return false;

  if (!_webui_browser_exist(win, Vivaldi))
    return false;

  if (!_webui_browser_create_profile_folder(win, Vivaldi))
    return false;

  char arg[1024] = {0};
  _webui_get_browser_args(win, Vivaldi, arg, sizeof(arg));

  char full[1024] = {0};
  sprintf(full, "%s%s%s", win->browser_path, arg, address);

  if (_webui_run_browser(win, full) == 0) {

    win->current_browser = Vivaldi;
    _webui_core.current_browser = Vivaldi;
    return true;
  } else
    return false;
}

static bool _webui_browser_start_brave(_webui_window_t *win,
                                       const char *address) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start_brave([%s])...\n", address);
#endif

  // -- Brave Browser ----------------------

  if (win->current_browser != 0 && win->current_browser != Brave)
    return false;

  if (!_webui_browser_exist(win, Brave))
    return false;

  if (!_webui_browser_create_profile_folder(win, Brave))
    return false;

  char arg[1024] = {0};
  _webui_get_browser_args(win, Brave, arg, sizeof(arg));

  char full[1024] = {0};
  sprintf(full, "%s%s%s", win->browser_path, arg, address);

  if (_webui_run_browser(win, full) == 0) {

    win->current_browser = Brave;
    _webui_core.current_browser = Brave;
    return true;
  } else
    return false;
}

static bool _webui_browser_start_firefox(_webui_window_t *win,
                                         const char *address) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start_firefox([%s])...\n", address);
#endif

  // -- Mozilla Firefox ----------------------

  if (win->current_browser != 0 && win->current_browser != Firefox)
    return false;

  if (!_webui_browser_exist(win, Firefox))
    return false;

  if (!_webui_browser_create_profile_folder(win, Firefox))
    return false;

  char arg[1024] = {0};
  _webui_get_browser_args(win, Firefox, arg, sizeof(arg));

  char full[1024] = {0};
  sprintf(full, "%s%s%s", win->browser_path, arg, address);

  if (_webui_run_browser(win, full) == 0) {

    win->current_browser = Firefox;
    _webui_core.current_browser = Firefox;
    return true;
  } else
    return false;
}

static bool _webui_browser_start_yandex(_webui_window_t *win,
                                        const char *address) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start_yandex([%s])...\n", address);
#endif

  // -- Yandex Browser ----------------------

  if (win->current_browser != 0 && win->current_browser != Yandex)
    return false;

  if (!_webui_browser_exist(win, Yandex))
    return false;

  if (!_webui_browser_create_profile_folder(win, Yandex))
    return false;

  char arg[1024] = {0};
  _webui_get_browser_args(win, Yandex, arg, sizeof(arg));

  char full[1024] = {0};
  sprintf(full, "%s%s%s", win->browser_path, arg, address);

  if (_webui_run_browser(win, full) == 0) {

    win->current_browser = Yandex;
    _webui_core.current_browser = Yandex;
    return true;
  } else
    return false;
}

static bool _webui_browser_start_chromium(_webui_window_t *win,
                                          const char *address) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start_chromium([%s])...\n", address);
#endif

  // -- The Chromium Projects -------------------

  if (win->current_browser != 0 && win->current_browser != Chromium)
    return false;

  if (!_webui_browser_exist(win, Chromium))
    return false;

  if (!_webui_browser_create_profile_folder(win, Chromium))
    return false;

  char arg[1024] = {0};
  _webui_get_browser_args(win, Chromium, arg, sizeof(arg));

  char full[1024] = {0};
  sprintf(full, "%s%s%s", win->browser_path, arg, address);

  if (_webui_run_browser(win, full) == 0) {

    win->current_browser = Chromium;
    _webui_core.current_browser = Chromium;
    return true;
  } else
    return false;
}

static bool _webui_browser_start(_webui_window_t *win, const char *address,
                                 size_t browser) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_browser_start([%s], [%zu])...\n", address, browser);
#endif

  // Non existing browser
  if (browser > 10)
    return false;

  // Current browser used in the last opened window
  if (browser == AnyBrowser && _webui_core.current_browser != 0)
    browser = _webui_core.current_browser;

  // TODO: Convert address from [/...] to [file://...]

  if (browser != 0) {

    // Open the window using the user specified browser

    if (browser == Chrome)
      return _webui_browser_start_chrome(win, address);
    else if (browser == Edge)
      return _webui_browser_start_edge(win, address);
    else if (browser == Epic)
      return _webui_browser_start_epic(win, address);
    else if (browser == Vivaldi)
      return _webui_browser_start_vivaldi(win, address);
    else if (browser == Brave)
      return _webui_browser_start_brave(win, address);
    else if (browser == Firefox)
      return _webui_browser_start_firefox(win, address);
    else if (browser == Yandex)
      return _webui_browser_start_yandex(win, address);
    else if (browser == Chromium)
      return _webui_browser_start_chromium(win, address);
    else
      return false;
  } else if (win->current_browser != 0) {

    // Open the window using the same web browser used for this current window

    if (win->current_browser == Chrome)
      return _webui_browser_start_chrome(win, address);
    else if (win->current_browser == Edge)
      return _webui_browser_start_edge(win, address);
    else if (win->current_browser == Epic)
      return _webui_browser_start_epic(win, address);
    else if (win->current_browser == Vivaldi)
      return _webui_browser_start_vivaldi(win, address);
    else if (win->current_browser == Brave)
      return _webui_browser_start_brave(win, address);
    else if (win->current_browser == Firefox)
      return _webui_browser_start_firefox(win, address);
    else if (win->current_browser == Yandex)
      return _webui_browser_start_yandex(win, address);
    else if (browser == Chromium)
      return _webui_browser_start_chromium(win, address);
    else
      return false;
  } else {

    // Open the window using the default OS browser

    // #1 - Chrome - Works perfectly
    // #2 - Edge - Works perfectly like Chrome
    // #3 - Epic - Works perfectly like Chrome
    // #4 - Vivaldi - Works perfectly like Chrome
    // #5 - Brave - Shows a policy notification in the first run
    // #6 - Firefox - Does not support App-Mode like Chrome (Looks not great)
    // #7 - Yandex - Shows a big welcome window in the first run
    // #8 - Chromium - Some Anti-Malware shows a false alert when using
    // ungoogled-chromium-binaries

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // Windows
    if (!_webui_browser_start_chrome(win, address))
      if (!_webui_browser_start_edge(win, address))
        if (!_webui_browser_start_epic(win, address))
          if (!_webui_browser_start_vivaldi(win, address))
            if (!_webui_browser_start_brave(win, address))
              if (!_webui_browser_start_firefox(win, address))
                if (!_webui_browser_start_yandex(win, address))
                  if (!_webui_browser_start_chromium(win, address))
                    return false;
#elif __APPLE__
    // macOS
    if (!_webui_browser_start_chrome(win, address))
      if (!_webui_browser_start_edge(win, address))
        if (!_webui_browser_start_epic(win, address))
          if (!_webui_browser_start_vivaldi(win, address))
            if (!_webui_browser_start_brave(win, address))
              if (!_webui_browser_start_firefox(win, address))
                if (!_webui_browser_start_yandex(win, address))
                  if (!_webui_browser_start_chromium(win, address))
                    return false;
#else
    // Linux
    if (!_webui_browser_start_chrome(win, address))
      if (!_webui_browser_start_edge(win, address))
        if (!_webui_browser_start_epic(win, address))
          if (!_webui_browser_start_vivaldi(win, address))
            if (!_webui_browser_start_brave(win, address))
              if (!_webui_browser_start_firefox(win, address))
                if (!_webui_browser_start_yandex(win, address))
                  if (!_webui_browser_start_chromium(win, address))
                    return false;
#endif
  }

  return true;
}

static bool _webui_is_process_running(const char *process_name) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_is_process_running([%s])...\n", process_name);
#endif

  bool isRunning = false;

#ifdef _WIN32
  // Microsoft Windows
  HANDLE hSnapshot;
  PROCESSENTRY32 pe32;
  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return false;
  pe32.dwSize = sizeof(PROCESSENTRY32);
  if (!Process32First(hSnapshot, &pe32)) {
    CloseHandle(hSnapshot);
    return false;
  }
  do {
    if (strcmp(pe32.szExeFile, process_name) == 0) {
      isRunning = true;
      break;
    }
  } while (Process32Next(hSnapshot, &pe32));
  CloseHandle(hSnapshot);
#elif __linux__
  // Linux
  DIR *dir;
  struct dirent *entry;
  char status_path[WEBUI_MAX_PATH];
  char line[WEBUI_MAX_PATH];
  dir = opendir("/proc");
  if (!dir)
    return false; // Unable to open /proc
  while ((entry = readdir(dir))) {
    if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
      snprintf(status_path, sizeof(status_path), "/proc/%s/status",
               entry->d_name);
      FILE *status_file = fopen(status_path, "r");
      if (status_file) {
        while (fgets(line, sizeof(line), status_file)) {
          if (strncmp(line, "Name:", 5) == 0) {
            char proc_name[WEBUI_MAX_PATH];
            sscanf(line, "Name: %s", proc_name);
            if (strcmp(proc_name, process_name) == 0) {
              isRunning = true;
              fclose(status_file);
              goto _close_dir;
            }
            break;
          }
        }
        fclose(status_file);
      }
    }
  }
_close_dir:
  closedir(dir);
#else
  // macOS
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
  struct kinfo_proc *procs = NULL;
  size_t size;
  if (sysctl(mib, 4, NULL, &size, NULL, 0) < 0)
    return false; // Failed to get process list size
  procs = (struct kinfo_proc *)malloc(size);
  if (!procs)
    return false; // Failed to allocate memory for process list
  if (sysctl(mib, 4, procs, &size, NULL, 0) < 0) {
    free(procs);
    return false; // Failed to get process list
  }
  size_t count = size / sizeof(struct kinfo_proc);
  for (size_t i = 0; i < count; i++) {
    if (strcmp(procs[i].kp_proc.p_comm, process_name) == 0) {
      isRunning = true;
      break;
    }
  }
  free(procs);
#endif

  return isRunning;
}

static size_t _webui_find_the_best_browser(_webui_window_t *win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_find_the_best_browser()...\n");
#endif

  // #1 - Chrome - Works perfectly
  // #2 - Edge - Works perfectly like Chrome
  // #3 - Epic - Works perfectly like Chrome
  // #4 - Vivaldi - Works perfectly like Chrome
  // #5 - Brave - Shows a policy notification in the first run
  // #6 - Firefox - Does not support App-Mode like Chrome (Looks not great)
  // #7 - Yandex - Shows a big welcome window in the first run
  // #8 - Chromium - Some Anti-Malware shows a false alert when using
  // ungoogled-chromium-binaries

  // To save memory, let's search if a web browser is already running

#ifdef _WIN32
  // Microsoft Windows
  if (_webui_is_process_running("chrome.exe") &&
      _webui_browser_exist(win, Chrome))
    return Chrome;
  else if (_webui_is_process_running("msedge.exe") &&
           _webui_browser_exist(win, Edge))
    return Edge;
  else if (_webui_is_process_running("epic.exe") &&
           _webui_browser_exist(win, Epic))
    return Epic;
  else if (_webui_is_process_running("vivaldi.exe") &&
           _webui_browser_exist(win, Vivaldi))
    return Vivaldi;
  else if (_webui_is_process_running("brave.exe") &&
           _webui_browser_exist(win, Brave))
    return Brave;
  else if (_webui_is_process_running("firefox.exe") &&
           _webui_browser_exist(win, Firefox))
    return Firefox;
  else if (_webui_is_process_running("browser.exe") &&
           _webui_browser_exist(win, Yandex))
    return Yandex;
  // Chromium check is never reached if Google Chrome is installed
  // due to duplicate process name `chrome.exe`
  else if (_webui_is_process_running("chrome.exe") &&
           _webui_browser_exist(win, Chromium))
    return Chromium;
#elif __linux__
  // Linux
  if (_webui_is_process_running("chrome") && _webui_browser_exist(win, Chrome))
    return Chrome;
  else if (_webui_is_process_running("msedge") &&
           _webui_browser_exist(win, Edge))
    return Edge;
  // Epic...
  else if (_webui_is_process_running("vivaldi-bin") &&
           _webui_browser_exist(win, Vivaldi))
    return Vivaldi;
  else if (_webui_is_process_running("brave") &&
           _webui_browser_exist(win, Brave))
    return Brave;
  else if (_webui_is_process_running("firefox") &&
           _webui_browser_exist(win, Firefox))
    return Firefox;
  else if (_webui_is_process_running("yandex_browser") &&
           _webui_browser_exist(win, Yandex))
    return Yandex;
  // Chromium check is never reached if Google Chrome is installed
  // due to duplicate process name `chrome`
  else if (_webui_is_process_running("chrome") &&
           _webui_browser_exist(win, Chromium))
    return Chromium;
#else
  // macOS
  if (_webui_is_process_running("Google Chrome") &&
      _webui_browser_exist(win, Chrome))
    return Chrome;
  else if (_webui_is_process_running("Epic") && _webui_browser_exist(win, Epic))
    return Epic;
  else if (_webui_is_process_running("Vivaldi") &&
           _webui_browser_exist(win, Vivaldi))
    return Vivaldi;
  else if (_webui_is_process_running("Brave") &&
           _webui_browser_exist(win, Brave))
    return Brave;
  else if (_webui_is_process_running("Firefox") &&
           _webui_browser_exist(win, Firefox))
    return Firefox;
  else if (_webui_is_process_running("Yandex") &&
           _webui_browser_exist(win, Yandex))
    return Yandex;
  else if (_webui_is_process_running("Chromium") &&
           _webui_browser_exist(win, Chromium))
    return Chromium;
#endif

  return AnyBrowser;
}

static bool _webui_show(_webui_window_t *win, const char *content,
                        size_t browser) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_show([%zu])...\n", browser);
#endif

  if (_webui_is_empty(content))
    return false;

  // Some wrappers does not guarantee `content` to
  // stay valid, so, let's make our copy right now
  size_t content_len = _webui_strlen(content);
  const char *content_cpy = (const char *)_webui_malloc(content_len);
  memcpy((char *)content_cpy, content, content_len);

  if (strstr(content_cpy, "<html")) {

// Embedded HTML
#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_show() -> Embedded HTML:\n");
    printf(
        "- - -[HTML]- - - - - - - - - -\n%s\n- - - - - - - - - - - - - - - -\n",
        content_cpy);
#endif

    return _webui_show_window(win, content_cpy, true, browser);
  } else {

// File
#ifdef WEBUI_LOG
    printf("[User] webui_show() -> File: [%s]\n", content_cpy);
#endif

    if (content_len > WEBUI_MAX_PATH || strstr(content_cpy, "<"))
      return false;

    return _webui_show_window(win, content_cpy, false, browser);
  }
}

static bool _webui_show_window(_webui_window_t *win, const char *content,
                               bool is_embedded_html, size_t browser) {

#ifdef WEBUI_LOG
  if (is_embedded_html)
    printf("[Core]\t\t_webui_show_window(HTML, [%zu])...\n", browser);
  else
    printf("[Core]\t\t_webui_show_window(FILE, [%zu])...\n", browser);
#endif

  _webui_init();

  char *url = NULL;
  size_t port =
      (win->server_port == 0 ? _webui_get_free_port() : win->server_port);
  size_t ws_port = (win->ws_port == 0 ? _webui_get_free_port() : win->ws_port);

  // Initialization
  if (win->html != NULL)
    _webui_free_mem((void *)win->html);
  if (win->url != NULL)
    _webui_free_mem((void *)win->url);

  if (is_embedded_html) {

    // Show a window using the embedded HTML
    win->is_embedded_html = true;
    win->html = (content == NULL ? webui_empty_string : content);

    // Generate the URL
    size_t url_len = 32; // [http][domain][port]
    url = (char *)_webui_malloc(url_len);
    sprintf(url, "http://localhost:%zu", port);
  } else {

    // Show a window using a local file
    win->is_embedded_html = false;
    win->html = NULL;

    // Generate the URL
    size_t url_len = 32 + _webui_strlen(content); // [http][domain][port][file]
    url = (char *)_webui_malloc(url_len);
    sprintf(url, "http://localhost:%zu/%s", port, content);
  }

  // Set URL
  win->url = url;
  win->server_port = port;
  win->ws_port = ws_port;

  if (!webui_is_shown(win->window_number)) {

    // Start a new window

    // Run browser
    if (!_webui_browser_start(win, win->url, browser)) {

      // Browser not available
      _webui_free_mem((void *)win->html);
      _webui_free_mem((void *)win->url);
      _webui_free_port(win->server_port);
      _webui_free_port(win->ws_port);
      return false;
    }

// New server thread
#ifdef _WIN32
    HANDLE thread =
        CreateThread(NULL, 0, _webui_server_start, (void *)win, 0, NULL);
    win->server_thread = thread;
    if (thread != NULL)
      CloseHandle(thread);
#else
    pthread_t thread;
    pthread_create(&thread, NULL, &_webui_server_start, (void *)win);
    pthread_detach(thread);
    win->server_thread = thread;
#endif
  } else {

    // Refresh an existing running window

    // Prepare packets
    size_t packet_len = 3 + _webui_strlen(url); // [header][url]
    char *packet = (char *)_webui_malloc(packet_len);
    packet[0] = WEBUI_HEADER_SIGNATURE;                  // Signature
    packet[1] = WEBUI_HEADER_SWITCH;                     // Type
    packet[2] = 0;                                       // ID
    for (size_t i = 0; i < _webui_strlen(win->url); i++) // URL
      packet[i + 3] = win->url[i];

    // Send the packet
    _webui_window_send(win, packet, packet_len);
    _webui_free_mem((void *)packet);
  }

  return true;
}

static void _webui_window_event(_webui_window_t *win, int event_type,
                                char *element, char *data, size_t event_number,
                                char *webui_internal_id) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_window_event([%s], [%s])...\n", webui_internal_id,
         element);
#endif

  // Create a thread, and call the used cb function
  // no need to wait for the response. This is fire
  // and forget.

  // Create a new CB args struct
  _webui_cb_arg_t *arg =
      (_webui_cb_arg_t *)_webui_malloc(sizeof(_webui_cb_arg_t));

  // Event
  arg->window = win;
  arg->event_type = event_type;
  arg->element = element;
  arg->data = data;
  arg->event_number = event_number;
  // Extras
  arg->webui_internal_id = webui_internal_id;

// fire and forget.
#ifdef _WIN32
  HANDLE user_fun_thread =
      CreateThread(NULL, 0, _webui_cb, (void *)arg, 0, NULL);
  if (user_fun_thread != NULL)
    CloseHandle(user_fun_thread);
#else
  pthread_t thread;
  pthread_create(&thread, NULL, &_webui_cb, (void *)arg);
  pthread_detach(thread);
#endif
}

static void _webui_window_send(_webui_window_t *win, char *packet,
                               size_t packets_size) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_window_send()...\n");
  printf("[Core]\t\t_webui_window_send() -> Packet size: %zu bytes \n",
         packets_size);
  printf("[Core]\t\t_webui_window_send() -> Packet hex : [ ");
  _webui_print_hex(packet, packets_size);
  printf("]\n");
  printf("[Core]\t\t_webui_window_send() -> Packet str : [%.*s] \n",
         (int)(packets_size - 3), (const char *)&packet[3]);
#endif

  if (!win->connected || packet == NULL || packets_size < 4)
    return;

  int ret = 0;

  if (win->window_number > 0 && win->window_number < WEBUI_MAX_ARRAY) {

    struct mg_connection *conn = _webui_core.mg_connections[win->window_number];

    if (conn != NULL) {
      ret = mg_websocket_write(conn, MG_WEBSOCKET_OPCODE_BINARY, packet,
                               packets_size);
    }
  }

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_window_send() -> %d bytes sent.\n", ret);
#endif
}

static bool _webui_get_data(const char *packet, size_t packet_len, size_t pos,
                            size_t *data_len, char **data) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_data()...\n");
#endif

  if ((pos + 1) > packet_len) {

    *data = (char *)webui_empty_string;
    *data_len = 0;
    return false;
  }

  // Calculate the data part size
  size_t data_size = _webui_strlen(&packet[pos]);
  if (data_size < 1) {

    *data = (char *)webui_empty_string;
    *data_len = 0;
    return false;
  }

  // Check the max packet size
  if (data_size > (packet_len - pos))
    data_size = (packet_len - pos);

  // Allocat mem
  *data = (char *)_webui_malloc(data_size);

  // Copy data part
  char *p = *data;
  size_t j = pos;
  for (size_t i = 0; i < data_size; i++) {

    memcpy(p, &packet[j], 1);
    p++;
    j++;
  }

  // Check data size
  *data_len = _webui_strlen(*data);
  if (*data_len < 1) {

    _webui_free_mem((void *)data);
    *data = (char *)webui_empty_string;
    *data_len = 0;
    return false;
  }

  return true;
}

static void _webui_window_receive(_webui_window_t *win, const char *packet,
                                  size_t len) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_window_receive()...\n");
#endif

  if ((unsigned char)packet[0] != WEBUI_HEADER_SIGNATURE || len < 4)
    return;

  if ((unsigned char)packet[1] == WEBUI_HEADER_CLICK) {

    // Click Event

    // 0: [Signature]
    // 1: [Type]
    // 2:
    // 3: [Data]

    // Get html element id
    char *element;
    size_t element_len;
    _webui_get_data(packet, len, 3, &element_len, &element);

#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_window_receive() -> WEBUI_HEADER_CLICK \n");
    printf("[Core]\t\t_webui_window_receive() -> Element size: %zu bytes \n",
           element_len);
    printf("[Core]\t\t_webui_window_receive() -> Element : [%s] \n", element);
#endif

    // Generate WebUI internal id
    char *webui_internal_id = _webui_generate_internal_id(win, element);

    _webui_window_event(win,                     // Event -> Window
                        WEBUI_EVENT_MOUSE_CLICK, // Event -> Type of this event
                        element,                 // Event -> HTML Element
                        NULL,                    // Event -> User Custom Data
                        0,                       // Event -> Event Number
                        webui_internal_id        // Extras -> WebUI Internal ID
    );
  } else if ((unsigned char)packet[1] == WEBUI_HEADER_JS) {

    // JS Result

    // 0: [Signature]
    // 1: [Type]
    // 2: [ID]
    // 3: [Error]
    // 4: [Data]

    // Get pipe id
    unsigned char run_id = packet[2];
    if (run_id < 0x01) {

      // Fatal.
      // The pipe ID is not valid
      // we can't send the ready signal to webui_script()
      return;
    }

    // Get data part
    char *data;
    size_t data_len;
    bool data_status = _webui_get_data(packet, len, 4, &data_len, &data);

    // Get js-error
    bool error = true;
    if ((unsigned char)packet[3] == 0x00)
      error = false;

#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_window_receive() -> WEBUI_HEADER_JS \n");
    printf("[Core]\t\t_webui_window_receive() -> run_id = 0x%02x \n", run_id);
    printf("[Core]\t\t_webui_window_receive() -> error = 0x%02x \n", error);
    printf("[Core]\t\t_webui_window_receive() -> %zu bytes of data\n",
           data_len);
    printf("[Core]\t\t_webui_window_receive() -> data = [%s] @ 0x%p\n", data,
           data);
#endif

    // Initialize pipe
    if ((void *)_webui_core.run_responses[run_id] != NULL)
      _webui_free_mem((void *)_webui_core.run_responses[run_id]);

    // Set pipe
    if (data_status && data_len > 0) {

      _webui_core.run_error[run_id] = error;
      _webui_core.run_responses[run_id] = data;
    } else {

      // Empty Result
      _webui_core.run_error[run_id] = error;
      _webui_core.run_responses[run_id] = webui_empty_string;
    }

    // Send ready signal to webui_script()
    _webui_core.run_done[run_id] = true;
  } else if ((unsigned char)packet[1] == WEBUI_HEADER_CALL_INTERNAL) {

    // JS Payload

    // 0: [Signature]
    // 1: [Type]
    // 2: [ID]
    // 3: [Error]
    // 4: [Data_len]
    // 5: [Data]

    // Get pipe id
    unsigned char run_id = packet[2];
    if (run_id < 0x01) {

      // Fatal.
      // The pipe ID is not valid
      // we can't send the ready signal to webui_script()
      return;
    }

    // Get data part
    size_t data_len = packet[4];
    char *data = _webui_malloc(data_len);
    for (size_t i = 0; i < data_len; i++) {
      data[i] = (unsigned char)packet[i + 5];
    }

    // Get js-error
    bool error = true;
    if ((unsigned char)packet[3] == 0x00)
      error = false;

#ifdef WEBUI_LOG
    printf(
        "[Core]\t\t_webui_window_receive() -> WEBUI_HEADER_CALL_INTERNAL \n");
    printf("[Core]\t\t_webui_window_receive() -> run_id = 0x%02x \n", run_id);
    printf("[Core]\t\t_webui_window_receive() -> error = 0x%02x \n", error);
    printf("[Core]\t\t_webui_window_receive() -> %zu bytes of data\n",
           data_len);
    printf("[Core]\t\t_webui_window_receive() -> data = [%s] @ 0x%p\n", data,
           data);
#endif

    // Initialize pipe
    if ((void *)_webui_core.run_responses[run_id] != NULL)
      _webui_free_mem((void *)_webui_core.run_responses[run_id]);

    // Set pipe
    if (data_len > 0) {

      _webui_core.run_error[run_id] = error;
      _webui_core.run_responses[run_id] = data;
    } else {

      // Empty Result
      _webui_core.run_error[run_id] = error;
      _webui_core.run_responses[run_id] = webui_empty_string;
    }

    // Send ready signal to webui_script()
    _webui_core.run_done[run_id] = true;
  } else if ((unsigned char)packet[1] == WEBUI_HEADER_SWITCH) {

    // Navigation Event

    // 0: [Signature]
    // 1: [Type]
    // 2:
    // 3: [URL]

    // Events
    if (win->has_events) {

      // Get URL
      char *url;
      size_t url_len;
      if (!_webui_get_data(packet, len, 3, &url_len, &url))
        return;

#ifdef WEBUI_LOG
      printf("[Core]\t\t_webui_window_receive() -> WEBUI_HEADER_SWITCH \n");
      printf("[Core]\t\t_webui_window_receive() -> URL size: %zu bytes \n",
             url_len);
      printf("[Core]\t\t_webui_window_receive() -> URL: [%s] \n", url);
#endif

      // Generate WebUI internal id
      char *webui_internal_id = _webui_generate_internal_id(win, "");

      _webui_window_event(win,                    // Event -> Window
                          WEBUI_EVENT_NAVIGATION, // Event -> Type of this event
                          "",                     // Event -> HTML Element
                          url,                    // Event -> User Custom Data
                          0,                      // Event -> Event Number
                          webui_internal_id       // Extras -> WebUI Internal ID
      );
    }
  } else if ((unsigned char)packet[1] == WEBUI_HEADER_CALL_FUNC) {

    // Function Call

    // 0: [Signature]
    // 1: [Type]
    // 2: [Call ID]
    // 3: [Element ID, Null, Data]

    // Get html element id
    char *element;
    size_t element_len;
    if (!_webui_get_data(packet, len, 3, &element_len, &element))
      return;

    // Get data
    char *data;
    size_t data_len;
    _webui_get_data(packet, len, (3 + element_len + 1), &data_len, &data);

#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_window_receive() -> WEBUI_HEADER_CALL_FUNC \n");
    printf("[Core]\t\t_webui_window_receive() -> Call ID: [0x%02x] \n",
           packet[2]);
    printf("[Core]\t\t_webui_window_receive() -> Element: [%s] \n", element);
    printf("[Core]\t\t_webui_window_receive() -> Data size: %zu Bytes \n",
           data_len);
    printf("[Core]\t\t_webui_window_receive() -> Data: [%s] \n", data);
#endif

    // Generate WebUI internal id
    char *webui_internal_id = _webui_generate_internal_id(win, element);

    // Create new event core to hold the response
    webui_event_core_t *event_core =
        (webui_event_core_t *)_webui_malloc(sizeof(webui_event_core_t));
    size_t event_core_pos = _webui_get_free_event_core_pos(win);
    win->event_core[event_core_pos] = event_core;
    char **response = &win->event_core[event_core_pos]->response;

    // Create new event
    webui_event_t e;
    e.window = win->window_number;
    e.event_type = WEBUI_EVENT_CALLBACK;
    e.element = element;
    e.data = data;
    e.event_number = event_core_pos;

    // Call user function
    size_t cb_index = _webui_get_cb_index(webui_internal_id);
    if (cb_index > 0 && _webui_core.cb[cb_index] != NULL) {

// Call user cb
#ifdef WEBUI_LOG
      printf(
          "[Core]\t\t_webui_window_receive() -> Calling user callback...\n\n");
#endif
      _webui_core.cb[cb_index](&e);
    }

    // Check the response
    if (_webui_is_empty(*response))
      *response = (char *)webui_empty_string;

#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_window_receive() -> user-callback response [%s]\n",
           *response);
#endif

    // 0: [Signature]
    // 1: [Type]
    // 2: [Call ID]
    // 3: [Data]

    // Prepare response packet
    size_t response_len = _webui_strlen(*response);
    size_t response_packet_len = 3 + response_len + 1;
    char *response_packet = (char *)_webui_malloc(response_packet_len);
    response_packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
    response_packet[1] = WEBUI_HEADER_CALL_FUNC; // Type
    response_packet[2] = packet[2];              // Call ID
    for (size_t i = 0; i < response_len; i++)    // Data
      response_packet[3 + i] = (*response)[i];

    // Send response packet
    _webui_window_send(win, response_packet, response_packet_len);
    _webui_free_mem((void *)response_packet);

    // Free
    _webui_free_mem((void *)element);
    _webui_free_mem((void *)data);
    _webui_free_mem((void *)webui_internal_id);
    _webui_free_mem((void *)*response);
    _webui_free_mem((void *)event_core);
  }
}

static char *_webui_get_current_path(void) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_current_path()...\n");
#endif

  char *path = (char *)_webui_malloc(WEBUI_MAX_PATH);
  if (WEBUI_GET_CURRENT_DIR(path, WEBUI_MAX_PATH) == NULL)
    path[0] = 0x00;

  return path;
}

static void _webui_free_port(size_t port) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_free_port([%zu])...\n", port);
#endif

  for (size_t i = 0; i < WEBUI_MAX_ARRAY; i++) {
    if (_webui_core.used_ports[i] == port) {
      _webui_core.used_ports[i] = 0;
      break;
    }
  }
}

static void _webui_wait_for_startup(void) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_wait_for_startup()...\n");
#endif

  if (_webui_core.connections > 0)
    return;

  // Wait for the first http request
  // while the web browser is starting up
  for (size_t n = 0; n < (_webui_core.startup_timeout * 20); n++) {
    // User/Default timeout
    if (_webui_core.server_handled)
      break;
    _webui_sleep(50);
  }

  // Wait for the first connection
  // while the WS is connecting
  if (_webui_core.wins[1] != NULL) {
    // 1500ms
    for (size_t n = 0; n < 30; n++) {
      if (_webui_core.connections > 0)
        break;
      _webui_sleep(50);
    }
  }

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_wait_for_startup() -> Finish.\n");
#endif
}

static size_t _webui_get_free_port(void) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_free_port()...\n");
#endif

#ifdef _WIN32
  srand((size_t)time(NULL));
#else
  srand(time(NULL));
#endif

  size_t port =
      (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;

  for (size_t i = WEBUI_MIN_PORT; i <= WEBUI_MAX_PORT; i++) {

    // Search [port] in [_webui_core.used_ports]
    bool found = false;
    for (size_t j = 0; j < WEBUI_MAX_ARRAY; j++) {
      if (_webui_core.used_ports[j] == port) {
        found = true;
        break;
      }
    }

    if (found)
      // Port used by local window
      port = (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;
    else {
      if (_webui_port_is_used(port))
        // Port used by an external app
        port =
            (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;
      else
        // Port is free
        break;
    }
  }

  // Add
  for (size_t i = 0; i < WEBUI_MAX_ARRAY; i++) {
    if (_webui_core.used_ports[i] == 0) {
      _webui_core.used_ports[i] = port;
      break;
    }
  }

  return port;
}

static void _webui_init(void) {

  if (_webui_core.initialized)
    return;

#ifdef WEBUI_LOG
  printf("[Core]\t\tWebUI v%s \n", WEBUI_VERSION);
  printf("[Core]\t\t_webui_init()...\n");
#endif

  // Initializing core
  memset(&_webui_core, 0, sizeof(_webui_core_t));
  _webui_core.initialized = true;
  _webui_core.startup_timeout = WEBUI_DEF_TIMEOUT;
  _webui_core.executable_path = _webui_get_current_path();

  // Initializing server services
  mg_init_library(0);
}

static size_t _webui_get_cb_index(char *webui_internal_id) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_cb_index([%s])...\n", webui_internal_id);
#endif

  if (webui_internal_id != NULL) {

    for (size_t i = 1; i < WEBUI_MAX_ARRAY; i++) {

      if (_webui_core.html_elements[i] != NULL &&
          !_webui_is_empty(_webui_core.html_elements[i]))
        if (strcmp(_webui_core.html_elements[i], webui_internal_id) == 0)
          return i;
    }
  }

  return 0;
}

static size_t _webui_set_cb_index(char *webui_internal_id) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_set_cb_index([%s])...\n", webui_internal_id);
#endif

  // Add
  for (size_t i = 1; i < WEBUI_MAX_ARRAY; i++) {

    if (_webui_is_empty(_webui_core.html_elements[i])) {

      _webui_core.html_elements[i] = webui_internal_id;

      return i;
    }
  }

  return 0;
}

#ifdef WEBUI_LOG
static void _webui_print_hex(const char *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    printf("0x%02X ", (unsigned char)*data);
    data++;
  }
}
static void _webui_print_ascii(const char *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    printf("%c ", (unsigned char)*data);
    data++;
  }
}
#endif

// HTTP Server

static void _webui_http_send(struct mg_connection *conn, const char *mime_type,
                             const char *body) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_http_send()...\n");
#endif

  size_t len = _webui_strlen(body);

  if (len < 1)
    return;

  // Send header
  int header_ret = mg_send_http_ok(conn, // 200
                                   mime_type, len);

  // Send body
  int body_ret = mg_write(conn, body, len);
}

static void _webui_http_send_error_page(struct mg_connection *conn,
                                        const char *body, int status) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_http_send_error_page()...\n");
#endif

  // Send header
  mg_response_header_start(conn, status);
  mg_response_header_add(conn, "Content-Type", "text/html; charset=utf-8", -1);
  mg_response_header_add(
      conn, "Cache-Control",
      "no-cache, no-store, must-revalidate, private, max-age=0", -1);
  mg_response_header_add(conn, "Pragma", "no-cache", -1);
  mg_response_header_add(conn, "Expires", "0", -1);
  mg_response_header_send(conn);

  // Send body
  mg_write(conn, body, strlen(body));
}

static int _webui_http_log(const struct mg_connection *conn,
                           const char *message) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_http_log()...\n");
  printf("[Core]\t\t_webui_http_log() -> Log: %s.\n", message);
#endif

  return 1;
}

static int _webui_http_handler(struct mg_connection *conn, void *_win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_http_handler()...\n");
#endif

  // Get the window object
  _webui_window_t *win = _webui_dereference_win_ptr(_win);
  if (win == NULL)
    return 500; // Internal Server Error

  // Initializing
  _webui_core.server_handled = true; // Main app wait
  win->server_handled = true;        // Window server wait
  int http_status_code = 200;

  const struct mg_request_info *ri = mg_get_request_info(conn);
  const char *url = ri->local_uri;

  if (strcmp(ri->request_method, "GET") == 0) {

    // GET

    // Let the server thread wait
    // more time for WS connection
    // as it may the UI have many
    // files to handel before first
    // ws connection established
    win->file_handled = true;

#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_http_handler() -> GET [%s]\n", url);
#endif

    if (strcmp(url, "/webui.js") == 0) {

      // WebUI JS-Bridge

#ifdef WEBUI_LOG
      printf("[Core]\t\t_webui_http_handler() -> HTML WebUI JS\n");
#endif

      // Generate JavaScript bridge
      const char *js = _webui_generate_js_bridge(win);

      // Send
      _webui_http_send(conn, // 200
                       "application/javascript", js);

      _webui_free_mem((void *)js);
    } else if (strcmp(url, "/") == 0) {

      // [/]

      if (win->is_embedded_html) {

        // Main HTML

        if (!win->multi_access && win->html_handled) {

          // Main HTML already handled.
          // Forbidden 403

#ifdef WEBUI_LOG
          printf("[Core]\t\t_webui_http_handler() -> Embedded Index HTML "
                 "Already Handled (403)\n");
#endif

          _webui_http_send_error_page(conn, webui_html_served, 403);

          http_status_code = 403;
        } else {

          // Send main HTML

          win->html_handled = true;

#ifdef WEBUI_LOG
          printf("[Core]\t\t_webui_http_handler() -> Embedded Index HTML\n");
#endif

          char *html = (char *)webui_empty_string;

          if (win->html != NULL) {

            // Generate the full WebUI JS-Bridge
            const char *js = _webui_generate_js_bridge(win);

            // Inject WebUI JS-Bridge into HTML
            size_t len = _webui_strlen(win->html) + 128;
            html = (char *)_webui_malloc(len);
            if (win->html != NULL && js != NULL) {
              sprintf(
                  html,
                  //! Construct bad html to load js bridge before all user
                  //! content Temp fix, need to improve this trick by inserting
                  //! the script tag in html head via an XML/DOM parser
                  "<html> <script type = \"application/javascript\" src = \" "
                  "webui.js \"> \n \n </script> \n %s",
                  win->html);
            }

            _webui_free_mem((void *)js);
          }

          // Send
          _webui_http_send(conn, // 200
                           "text/html", html);

          _webui_free_mem((void *)html);
        }
      } else {

        // Serve as index local file

        win->html_handled = true;

#ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_http_handler() -> Local Index File\n");
#endif

        // Set full path
        // [Path][Sep][File Name]
        char *index =
            (char *)_webui_malloc(_webui_strlen(win->server_root_path) + 1 + 8);

        // Index.ts
        sprintf(index, "%s%sindex.ts", win->server_root_path, webui_sep);
        if (_webui_file_exist(index)) {

          // TypeScript Index
          if (win->runtime != None)
            http_status_code = _webui_interpret_file(win, conn, index);
          else
            http_status_code = _webui_serve_file(win, conn);

          _webui_free_mem((void *)index);
          return 0;
        }

        // Index.js
        sprintf(index, "%s%sindex.js", win->server_root_path, webui_sep);
        if (_webui_file_exist(index)) {

          // JavaScript Index
          if (win->runtime != None)
            http_status_code = _webui_interpret_file(win, conn, index);
          else
            http_status_code = _webui_serve_file(win, conn);

          _webui_free_mem((void *)index);
          return 0;
        }

        _webui_free_mem((void *)index);

        // Index.html
        // Serve as a normal HTML text-based file
        http_status_code = _webui_serve_file(win, conn);
      }
    } else if (strcmp(url, "/favicon.ico") == 0 ||
               strcmp(url, "/favicon.svg") == 0) {

      // Favicon

      if (win->icon != NULL && win->icon_type != NULL) {

        // Custom user icon

        // User icon
        _webui_http_send(conn, // 200
                         win->icon_type, win->icon);
      } else if (_webui_file_exist_mg(win, conn)) {

        // Local icon file
        http_status_code = _webui_serve_file(win, conn);
      } else {

        // Default embedded icon

        if (strcmp(url, "/favicon.ico") == 0) {

          mg_send_http_redirect(conn, "favicon.svg", 302);
          http_status_code = 302;
        } else {

          // Default icon
          _webui_http_send(conn, // 200
                           webui_def_icon_type, webui_def_icon);
        }
      }
    } else {

      // [/file]

      if (win->runtime != None) {

#ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_http_handler() -> Trying to interpret local "
               "file\n");
#endif

        http_status_code = _webui_interpret_file(win, conn, NULL);
      } else {

#ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_http_handler() -> Text based local file\n");
#endif

        // Serve as a normal text-based file
        http_status_code = _webui_serve_file(win, conn);
      }
    }
  } else {

#ifdef WEBUI_LOG
    printf("[Core]\t\t_webui_http_handler() -> Unknown request method [%s]\n",
           ri->request_method);
#endif
  }

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_http_handler() -> HTTP Status Code: %d\n",
         http_status_code);
#endif
  return http_status_code;
}

// WS Server

static int _webui_ws_connect_handler(const struct mg_connection *conn,
                                     void *_win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_ws_connect_handler()...\n");
#endif

  (void)_win; /* unused */

  // OK
  return 0;
}

static void _webui_ws_ready_handler(struct mg_connection *conn, void *_win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_ws_ready_handler()...\n");
#endif

  _webui_window_t *win = _webui_dereference_win_ptr(_win);
  if (win == NULL)
    return;

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_ws_ready_handler() -> WebSocket Connected\n");
#endif

  int event_type = WEBUI_EVENT_CONNECTED;

  if (!win->connected) {

    // First connection

    win->connected = true;     // server thread
    _webui_core.connections++; // main loop
    _webui_core.mg_connections[win->window_number] =
        conn; // websocket send func
  } else {

    if (win->multi_access) {

      // Multi connections
      win->connections++;
      event_type = WEBUI_EVENT_MULTI_CONNECTION;
    } else {

      // UNWANTED Multi connections

#ifdef WEBUI_LOG
      printf("[Core]\t\t_webui_ws_ready_handler() -> UNWANTED Multi "
             "Connections\n");
#endif

      mg_close_connection(conn);
      event_type = WEBUI_EVENT_UNWANTED_CONNECTION;
    }
  }

  // New Event
  if (win->has_events) {

    // Generate WebUI internal id
    char *webui_internal_id = _webui_generate_internal_id(win, "");

    _webui_window_event(win,              // Event -> Window
                        event_type,       // Event -> Type of this event
                        "",               // Event -> HTML Element
                        NULL,             // Event -> User Custom Data
                        0,                // Event -> Event Number
                        webui_internal_id // Extras -> WebUI Internal ID
    );
  }
}

static int _webui_ws_data_handler(struct mg_connection *conn, int opcode,
                                  char *data, size_t datasize, void *_win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_ws_data_handler()...\n");
#endif

  if (datasize < 1)
    return 1; // OK

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_ws_data_handler() -> Size : %zu bytes\n", datasize);
  printf("[Core]\t\t_webui_ws_data_handler() -> Hex  : [ ");
  _webui_print_hex(data, datasize);
  printf("]\n");
  printf("[Core]\t\t_webui_ws_data_handler() -> ASCII: [ ");
  _webui_print_ascii(data, datasize);
  printf("]\n");
#endif

  switch (opcode & 0xf) {

  case MG_WEBSOCKET_OPCODE_BINARY: {
    // Process the packet
    _webui_window_t *win = _webui_dereference_win_ptr(_win);
    if (win != NULL)
      _webui_window_receive(win, data, datasize);
    break;
  }
  case MG_WEBSOCKET_OPCODE_TEXT: {
    break;
  }
  case MG_WEBSOCKET_OPCODE_PING: {
    break;
  }
  case MG_WEBSOCKET_OPCODE_PONG: {
    break;
  }
  }

  // OK
  return 1;
}

static void _webui_ws_close_handler(const struct mg_connection *conn,
                                    void *_win) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_ws_close_handler()...\n");
#endif

  // Dereference
  _webui_window_t *win = _webui_dereference_win_ptr(_win);
  if (win == NULL || !win->connected)
    return;

  win->html_handled = false;
  win->server_handled = false;

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_ws_close_handler() -> WebSocket Closed\n");
#endif

  if (win->connected) {

    if (win->multi_access && win->connections > 0) {

      // Multi connections close
      win->connections--;
    } else {

      // Main connection close
      _webui_core.connections--; // main loop
      win->connected = false;    // server thread
    }
  }

  // Events
  if (win->has_events) {

    // Generate WebUI internal id
    char *webui_internal_id = _webui_generate_internal_id(win, "");

    _webui_window_event(win,                      // Event -> Window
                        WEBUI_EVENT_DISCONNECTED, // Event -> Type of this event
                        "",                       // Event -> HTML Element
                        NULL,                     // Event -> User Custom Data
                        0,                        // Event -> Event Number
                        webui_internal_id         // Extras -> WebUI Internal ID
    );
  }
}

static WEBUI_SERVER_START {
#ifdef WEBUI_LOG
  printf("[Core]\t\t[Thread] _webui_server_start()...\n");
#endif

  _webui_window_t *win = _webui_dereference_win_ptr(arg);
  if (win == NULL) {
    THREAD_RETURN
  }

#ifdef WEBUI_LOG
  printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> URL: [%s]\n",
         win->window_number, win->url);
#endif

  // Initialization
  _webui_core.servers++;
  win->server_running = true;
  win->html_handled = false;
  win->server_handled = false;
  if (_webui_core.startup_timeout < 1)
    _webui_core.startup_timeout = 0;
  if (_webui_core.startup_timeout > 30)
    _webui_core.startup_timeout = 30;

  // HTTP Port
  char *server_port = (char *)_webui_malloc(16);
  sprintf(server_port, "%zu", win->server_port);

  // WS Port
  char *ws_port = (char *)_webui_malloc(16);
  sprintf(ws_port, "%zu", win->ws_port);

  // Start HTTP Server
  const char *http_options[] = {"listening_ports",
                                server_port,
                                "document_root",
                                win->server_root_path,
                                "access_control_allow_headers",
                                "*",
                                "access_control_allow_methods",
                                "*",
                                "access_control_allow_origin",
                                "*",
                                NULL,
                                NULL};
  struct mg_callbacks http_callbacks;
  struct mg_context *http_ctx;
  memset(&http_callbacks, 0, sizeof(http_callbacks));
  http_callbacks.log_message = _webui_http_log;
  http_ctx = mg_start(&http_callbacks, 0, http_options);
  mg_set_request_handler(http_ctx, "/", _webui_http_handler, (void *)win);

  // Start WS Server
  struct mg_callbacks ws_callbacks = {0};
  struct mg_init_data ws_mg_start_init_data = {0};
  ws_mg_start_init_data.callbacks = &ws_callbacks;
  ws_mg_start_init_data.user_data = (void *)win;
  const char *ws_server_options[] = {"listening_ports",
                                     ws_port,
                                     "document_root",
                                     "/_webui_ws_connect",
                                     "websocket_timeout_ms",
                                     "30000",
                                     "enable_websocket_ping_pong",
                                     "yes",
                                     NULL,
                                     NULL};
  ws_mg_start_init_data.configuration_options = ws_server_options;
  struct mg_error_data ws_mg_start_error_data = {0};
  char ws_errtxtbuf[256] = {0};
  ws_mg_start_error_data.text = ws_errtxtbuf;
  ws_mg_start_error_data.text_buffer_size = sizeof(ws_errtxtbuf);
  struct mg_context *ws_ctx =
      mg_start2(&ws_mg_start_init_data, &ws_mg_start_error_data);

  if (http_ctx && ws_ctx) {

    mg_set_websocket_handler(ws_ctx, "/_webui_ws_connect",
                             _webui_ws_connect_handler, _webui_ws_ready_handler,
                             _webui_ws_data_handler, _webui_ws_close_handler,
                             (void *)win);

    if (_webui_core.startup_timeout > 0) {

#ifdef WEBUI_LOG
      printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Listening "
             "Success\n",
             win->window_number);
      printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> HTTP Port: %s\n",
             win->window_number, server_port);
      printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> WS Port: %s\n",
             win->window_number, ws_port);
      printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Timeout is %zu "
             "seconds\n",
             win->window_number, _webui_core.startup_timeout);
      printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Root path: %s\n",
             win->window_number, win->server_root_path);
#endif

      bool stop = false;

      while (!stop) {

        if (!win->connected) {

#ifdef WEBUI_LOG
          printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Waiting for "
                 "first HTTP request...\n",
                 win->window_number);
#endif

          // Wait for first connection
          _webui_timer_t timer_1;
          _webui_timer_start(&timer_1);
          for (;;) {

            // Stop if window is connected
            _webui_sleep(1);
            if (win->connected || win->server_handled)
              break;

            // Stop if timer is finished
            // default is WEBUI_DEF_TIMEOUT (30 seconds)
            if (_webui_timer_is_end(&timer_1,
                                    (_webui_core.startup_timeout * 1000)))
              break;
          }

          if (!win->connected && win->server_handled) {

            // At this moment the browser is already started and HTML
            // is already handled, so, let's wait more time to give
            // the WebSocket an extra one and half second to connect.

            do {
#ifdef WEBUI_LOG
              printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Waiting "
                     "for first connection...\n",
                     win->window_number);
#endif

              win->file_handled = false;

              _webui_timer_t timer_2;
              _webui_timer_start(&timer_2);
              for (;;) {

                // Stop if window is connected
                _webui_sleep(1);
                if (win->connected)
                  break;

                // Stop if timer is finished
                if (_webui_timer_is_end(&timer_2, 1500))
                  break;
              }
            } while (win->file_handled && !win->connected);
          }

          if (!win->connected)
            stop = true; // First run failed
        } else {

          // UI is connected

#ifdef WEBUI_LOG
          printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Window "
                 "Connected.\n",
                 win->window_number);
#endif

          while (!stop) {

            // Wait forever for disconnection

            _webui_sleep(1);

            // Exit signal
            if (_webui_core.exit_now) {
              stop = true;
              break;
            }

            if (!win->connected && !_webui_core.exit_now) {

              // The UI is just get disconnected
              // probably the user did a refresh
              // let's wait for re-connection...

#ifdef WEBUI_LOG
              printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Window "
                     "disconnected\n",
                     win->window_number);
#endif

              do {
#ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> "
                       "Waiting for reconnection...\n",
                       win->window_number);
#endif

                win->file_handled = false;

                _webui_timer_t timer_3;
                _webui_timer_start(&timer_3);
                for (;;) {

                  // Stop if window is re-connected
                  _webui_sleep(1);
                  if (win->connected)
                    break;

                  // Stop if timer is finished
                  if (_webui_timer_is_end(&timer_3, 1000))
                    break;
                }
              } while (win->file_handled && !win->connected);

              if (!win->connected) {
                stop = true;
                break;
              }
            }
          }
        }
      }
    }

    // Let's check the flag again, there is a change that
    // the flag has ben changed during the first loop for
    // example when set_timeout() get called later
    if (_webui_core.startup_timeout == 0) {

#ifdef WEBUI_LOG
      printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Listening "
             "success\n",
             win->window_number);
      printf(
          "[Core]\t\t[Thread] _webui_server_start([%zu]) -> Infinite loop...\n",
          win->window_number);
#endif

      // Wait forever
      for (;;) {

        _webui_sleep(1);
        if (_webui_core.exit_now)
          break;
      }
    }
  } else {

#ifdef WEBUI_LOG
    printf(
        "[Core]\t\t[Thread] _webui_server_start([%zu]) -> Listening failed\n",
        win->window_number);
#endif
  }

#ifdef WEBUI_LOG
  printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Cleaning...\n",
         win->window_number);
#endif

  // Clean
  win->server_running = false;
  win->html_handled = false;
  win->server_handled = false;
  win->connected = false;
  _webui_free_port(win->server_port);
  _webui_free_port(win->ws_port);

  // Kill Process
  // _webui_kill_pid(win->process_id);
  // win->process_id = 0;

#ifdef WEBUI_LOG
  printf("[Core]\t\t[Thread] _webui_server_start([%zu]) -> Server stoped.\n",
         win->window_number);
#endif

  // Let the main wait() know that
  // this server thread is finished
  _webui_core.servers--;

  // Stop server services
  // This should be at the
  // end as it may take time
  mg_stop(ws_ctx);
  mg_stop(http_ctx);

  THREAD_RETURN
}

static WEBUI_CB {
#ifdef WEBUI_LOG
  printf("[Core]\t\t[Thread] _webui_cb()...\n");
#endif

  _webui_cb_arg_t *arg = (_webui_cb_arg_t *)_arg;

  // Event
  webui_event_t e;
  e.window = arg->window->window_number;
  e.event_type = arg->event_type;
  e.element = arg->element;
  e.data = arg->data;
  e.event_number = arg->event_number;

  // Check for all events-bind functions
  if (arg->window->has_events) {

    char *events_id = _webui_generate_internal_id(arg->window, "");
    size_t events_cb_index = _webui_get_cb_index(events_id);
    _webui_free_mem((void *)events_id);

    if (events_cb_index > 0 && _webui_core.cb[events_cb_index] != NULL) {

// Call user all events cb
#ifdef WEBUI_LOG
      printf("[Core]\t\t[Thread] _webui_cb() -> Calling user callback...\n\n");
#endif
      _webui_core.cb[events_cb_index](&e);
    }
  }

  // Check for the regular bind functions
  if (arg->element != NULL && !_webui_is_empty(arg->element)) {

    size_t cb_index = _webui_get_cb_index(arg->webui_internal_id);
    if (cb_index > 0 && _webui_core.cb[cb_index] != NULL) {

// Call user cb
#ifdef WEBUI_LOG
      printf("[Core]\t\t[Thread] _webui_cb() -> Calling user callback...\n\n");
#endif
      _webui_core.cb[cb_index](&e);
    }
  }

#ifdef WEBUI_LOG
  printf("[Core]\t\t[Thread] _webui_cb() -> Finished.\n");
#endif

  // Free event
  _webui_free_mem((void *)arg->element);
  _webui_free_mem((void *)arg->data);
  // Free event extras
  _webui_free_mem((void *)arg->webui_internal_id);
  _webui_free_mem((void *)arg);

  THREAD_RETURN
}

#ifdef _WIN32
static void _webui_kill_pid(size_t pid) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_kill_pid(%zu)...\n", pid);
#endif

  if (pid < 1)
    return;
  HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, (DWORD)pid);
  if (hProcess != NULL) {
    TerminateProcess(hProcess, 1);
    CloseHandle(hProcess);
  }
}
#else
static void _webui_kill_pid(size_t pid) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_kill_pid(%zu)...\n", pid);
#endif

  if (pid < 1)
    return;
  kill((pid_t)pid, SIGTERM);
}
#endif

#ifdef _WIN32

static bool _webui_socket_test_listen_win32(size_t port_num) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_socket_test_listen_win32([%zu])...\n", port_num);
#endif

  WSADATA wsaData;
  size_t iResult;
  SOCKET ListenSocket = INVALID_SOCKET;
  struct addrinfo *result = NULL;
  struct addrinfo hints;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    // WSAStartup failed
    return false;
  }
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the server address and port
  char the_port[16] = {0};
  sprintf(&the_port[0], "%zu", port_num);
  iResult = getaddrinfo("127.0.0.1", &the_port[0], &hints, &result);
  if (iResult != 0) {
    // WSACleanup();
    return false;
  }

  // Create a SOCKET for the server to listen for client connections.
  ListenSocket =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    freeaddrinfo(result);
    // WSACleanup();
    return false;
  }

  // Setup the TCP listening socket
  iResult = bind(ListenSocket, result->ai_addr, (size_t)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    freeaddrinfo(result);
    closesocket(ListenSocket);
    shutdown(ListenSocket, SD_BOTH);
    // WSACleanup();
    return false;
  }

  // Clean
  freeaddrinfo(result);
  closesocket(ListenSocket);
  shutdown(ListenSocket, SD_BOTH);
  // WSACleanup();

  // Listening Success
  return true;
}

static int _webui_system_win32_out(const char *cmd, char **output, bool show) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_system_win32_out()...\n");
#endif

  // Ini
  *output = NULL;
  if (cmd == NULL)
    return -1;

  // Return
  DWORD Return = 0;

  // Flags
  DWORD CreationFlags = CREATE_NO_WINDOW;
  if (show)
    CreationFlags = SW_SHOW;

  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;
  HANDLE stdout_read, stdout_write;
  if (!CreatePipe(&stdout_read, &stdout_write, &sa, 0)) {
    return -1;
  }
  if (!SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0)) {
    CloseHandle(stdout_read);
    CloseHandle(stdout_write);
    return -1;
  }

  STARTUPINFOA si;
  ZeroMemory(&si, sizeof(STARTUPINFOA));
  si.cb = sizeof(STARTUPINFOA);
  si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  si.wShowWindow = SW_HIDE;
  si.hStdOutput = stdout_write;
  si.hStdError = stdout_write;

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

  if (!CreateProcessA(NULL,          // No module name (use cmd line)
                      (LPSTR)cmd,    // Command line
                      NULL,          // Process handle not inheritable
                      NULL,          // Thread handle not inheritable
                      TRUE,          // Set handle inheritance to FALSE
                      CreationFlags, // Creation flags
                      NULL,          // Use parent's environment block
                      NULL,          // Use parent's starting directory
                      &si,           // Pointer to STARTUP INFO structure
                      &pi))          // Pointer to PROCESS_INFORMATION structure
  {
    CloseHandle(stdout_read);
    CloseHandle(stdout_write);
    return -1;
  }
  CloseHandle(stdout_write);

  SetFocus(pi.hProcess);
  WaitForSingleObject(pi.hProcess, INFINITE);
  GetExitCodeProcess(pi.hProcess, &Return);

  DWORD bytes_read;
  char buffer[WEBUI_CMD_STDOUT_BUF];
  size_t output_size = 0;

  while (
      ReadFile(stdout_read, buffer, WEBUI_CMD_STDOUT_BUF, &bytes_read, NULL) &&
      bytes_read > 0) {

    char *new_output = realloc(*output, output_size + bytes_read + 1);
    if (new_output == NULL) {
      free(*output);
      CloseHandle(stdout_read);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return -1;
    }

    *output = new_output;
    memcpy(*output + output_size, buffer, bytes_read);
    output_size += bytes_read;
  }

  if (*output != NULL)
    (*output)[output_size] = '\0';

  CloseHandle(stdout_read);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  if (Return == 0)
    return 0;
  else
    return -1;
}

static int _webui_system_win32(_webui_window_t *win, char *cmd, bool show) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_system_win32()...\n");
#endif

  DWORD Return = 0;
  DWORD CreationFlags = CREATE_NO_WINDOW;

  /*
  We should not kill this process, because may had many child
  process of other WebUI app instances. Unfortunately, this is
  how modern browsers save memory by combine all windows into one
  single parent process, and we can't control this behavior.

  // Automatically close the browser process when the
  // parent process (this app) get closed. If this fail
  // webui.js will try to close the window.
  HANDLE JobObject = CreateJobObject(NULL, NULL);
  JOB_OBJECT_EXTENDED_LIMIT_INFORMATION ExtendedInfo = { 0 };
  ExtendedInfo.BasicLimitInformation.LimitFlags =
  JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION |
  JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE; SetInformationJobObject(JobObject,
  JobObjectExtendedLimitInformation, &ExtendedInfo, sizeof(ExtendedInfo));
  */

  if (show)
    CreationFlags = SW_SHOW;

  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  if (!CreateProcessA(NULL,          // No module name (use command line)
                      cmd,           // Command line
                      NULL,          // Process handle not inheritable
                      NULL,          // Thread handle not inheritable
                      FALSE,         // Set handle inheritance to FALSE
                      CreationFlags, // Creation flags
                      NULL,          // Use parent's environment block
                      NULL,          // Use parent's starting directory
                      &si,           // Pointer to STARTUP INFO structure
                      &pi))          // Pointer to PROCESS_INFORMATION structure
  {
    // CreateProcess failed
    return -1;
  }

  win->process_id = (size_t)pi.dwProcessId;
  SetFocus(pi.hProcess);
  // AssignProcessToJobObject(JobObject, pi.hProcess);
  WaitForSingleObject(pi.hProcess, INFINITE);
  GetExitCodeProcess(pi.hProcess, &Return);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  if (Return == 0)
    return 0;
  else
    return -1;
}

static bool _webui_get_windows_reg_value(HKEY key, LPCWSTR reg,
                                         LPCWSTR value_name,
                                         char value[WEBUI_MAX_PATH]) {

#ifdef WEBUI_LOG
  printf("[Core]\t\t_webui_get_windows_reg_value([%Ls])...\n", reg);
#endif

  HKEY hKey;

  if (RegOpenKeyExW(key, reg, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

    DWORD VALUE_TYPE;
    BYTE VALUE_DATA[WEBUI_MAX_PATH];
    DWORD VALUE_SIZE = sizeof(VALUE_DATA);

    // If `value_name` is empty then it will read the "(default)" reg-key
    if (RegQueryValueExW(hKey, value_name, NULL, &VALUE_TYPE, VALUE_DATA,
                         &VALUE_SIZE) == ERROR_SUCCESS) {

      if (VALUE_TYPE == REG_SZ)
        sprintf(value, "%S", (LPCWSTR)VALUE_DATA);
      else if (VALUE_TYPE == REG_DWORD)
        sprintf(value, "%zu", *((DWORD *)VALUE_DATA));

      RegCloseKey(hKey);
      return true;
    }
  }

  return false;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
  return true;
}

#endif
