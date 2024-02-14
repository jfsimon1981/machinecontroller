/*
 * Project   Chrysalide Standard Library
 * Author    Jean-François Simon
 * Company   Chrysalide Engineering
 * Date      2024/02/14
 * Version   1.0
 */

/*
 *  Copyright 2024 Jean‐François Simon, Chrysalide Engineering
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright  notice,  this
 * list of conditions and the following disclaimer.
 *
 * 2.  Redistributions  in  binary  form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3.  Neither  the  name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from  this  software  without
 * specific prior written permission.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  TO,  THE  IMPLIED
 * WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAI‐
 * MED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  LIABLE  FOR  ANY
 * DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (IN‐
 * CLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  SERVICES;  LOSS
 * OF  USE,  DATA,  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR  TORT  (INCLUDING
 * NEGLIGENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "csl.h"


// ************* Push-button *************

csl::Push_button::Push_button() {}
csl::Push_button::~Push_button() {}

csl::Push_button::Push_button(std::string s_on_in, std::string s_off_in) {
    load_tex_on_off(s_on_in, s_off_in);
}

// Tex on/off, pos, scale (xy)
csl::Push_button::Push_button(std::string s_on_in, std::string s_off_in, sf::Vector2<float> pos_in, float scale_in) {
    load_tex_on_off(s_on_in, s_off_in);
    sprite.setPosition(pos_in);
    sprite.setScale(scale_in, scale_in);
}

void csl::Push_button::load_tex_on(std::string s_in) {
    if (!tex_on.loadFromFile(s_in)) {std::cout << "Error loading push button texture: " << s_in << std::endl; /*Err handling*/}
    else {
        sprite.setTexture(tex_on);
        state = 1;
    }
}

void csl::Push_button::load_tex_off(std::string s_in) {
    if (!tex_off.loadFromFile(s_in)) {std::cout << "Error loading push button texture: " << s_in << std::endl; /*Err handling*/}
    else {
        sprite.setTexture(tex_off);
        state = 0;
    }
}

void csl::Push_button::load_tex_on_off(std::string tex_on_in, std::string tex_off_in) {
    csl::Push_button::load_tex_on(tex_on_in);
    csl::Push_button::load_tex_off(tex_off_in);
}

void csl::Push_button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    //target.draw(sprite, states);
    target.draw(sprite);
}

void csl::Push_button::set_on() {
    sprite.setTexture(tex_on);
    state = 1;
}
void csl::Push_button::set_off() {
    sprite.setTexture(tex_off);
    state = 0;
}

int csl::Push_button::is_pressed(const sf::RenderWindow &win) {
    sf::Vector2<int> mp = sf::Mouse::getPosition(win);
    return sprite.getGlobalBounds().contains(mp.x, mp.y);
}

void csl::Push_button::set_test() {
    sprite.setTexture(tex_on);
}
void csl::Push_button::set_normal() {
    if (state) sprite.setTexture(tex_on);
    else sprite.setTexture(tex_off);
}


// ************* Seven Segment Digit *************

csl::Seven_seg_digit::Seven_seg_digit() {
    if (!dig_tex[0].loadFromFile("medias/7seg_0.png")) {std::cout << "Seven_seg_digit: error loading texture 0" << std::endl;};
    if (!dig_tex[1].loadFromFile("medias/7seg_1.png")) {std::cout << "Seven_seg_digit: error loading texture 1" << std::endl;};
    if (!dig_tex[2].loadFromFile("medias/7seg_2.png")) {std::cout << "Seven_seg_digit: error loading texture 2" << std::endl;};
    if (!dig_tex[3].loadFromFile("medias/7seg_3.png")) {std::cout << "Seven_seg_digit: error loading texture 3" << std::endl;};
    if (!dig_tex[4].loadFromFile("medias/7seg_4.png")) {std::cout << "Seven_seg_digit: error loading texture 4" << std::endl;};
    if (!dig_tex[5].loadFromFile("medias/7seg_5.png")) {std::cout << "Seven_seg_digit: error loading texture 5" << std::endl;};
    if (!dig_tex[6].loadFromFile("medias/7seg_6.png")) {std::cout << "Seven_seg_digit: error loading texture 6" << std::endl;};
    if (!dig_tex[7].loadFromFile("medias/7seg_7.png")) {std::cout << "Seven_seg_digit: error loading texture 7" << std::endl;};
    if (!dig_tex[8].loadFromFile("medias/7seg_8.png")) {std::cout << "Seven_seg_digit: error loading texture 8" << std::endl;};
    if (!dig_tex[9].loadFromFile("medias/7seg_9.png")) {std::cout << "Seven_seg_digit: error loading texture 9" << std::endl;};
    if (!dig_tex[10].loadFromFile("medias/7seg_off.png")) {std::cout << "Seven_seg_digit: error loading texture off" << std::endl;};
    set_dig(10);
}
void csl::Seven_seg_digit::set_dig(int din) {
    if (din < 0) din = 0;
    else if (din > 10) din = 10;
    dig_sprite.setTexture(dig_tex[din]);
}
void csl::Seven_seg_digit::draw(sf::RenderWindow *win) {
    win->draw(dig_sprite);
}
int csl::Seven_seg_digit::operator = (int vin) {
    set_dig(vin);
    return vin;
}


// ************* Seven Segment Display *************

void csl::Seven_seg_display::draw(sf::RenderWindow *win) {for (auto i:seven_seg_digits) i->draw(win);}

csl::Seven_seg_display::Seven_seg_display (unsigned int dig_count) {
    for (auto i {0}; i < dig_count; i++) {
        seven_seg_digits.emplace_back(std::make_shared<Seven_seg_digit>());
        seven_seg_digits[i]->dig_sprite.setPosition(h_spacing*i, 0);
    }
}

void csl::Seven_seg_display::set_digit(int dig, int value) {
    if (dig < 0) dig = 0;
    else if (dig >= seven_seg_digits.size()) dig = seven_seg_digits.size() - 1;
    //seven_seg_digits[dig]->set_dig(value);
    *seven_seg_digits[dig] = value;
}

void csl::Seven_seg_display::set_position (int x, int y) {
    int ix {0};
    for (auto i:seven_seg_digits) i->dig_sprite.setPosition(x - h_spacing * ix++, y);
}

void csl::Seven_seg_display::set_scale (const float scalex, const float scaley) {
    for (auto i:seven_seg_digits) i->dig_sprite.setScale(scalex,scaley);
}

void csl::Seven_seg_display::set_scale (const float scale) {set_scale(scale, scale);}

int csl::Seven_seg_display::operator = (int vin) {
    unsigned int di {1}; // di = 1, 10, 100, ...
    for (auto i:seven_seg_digits) {
        *i = vin / di % 10; // Extract a digits: vin / {1, 10, 100, ...} % 10
        di *= 10;
    }
    return vin;
}


// ************* Serial Communications *************

/*
 * TODO
 * - Inherit from Communications class
 * - Define communication invariants
 */

csl::Serial::Serial() {
    #ifdef __linux__
    printf("Open /dev/ttyACMx on linux\n\r");
    fd = open(pport_default_linux, O_RDWR | O_NOCTTY | O_NONBLOCK);
    #endif
    #ifdef __OpenBSD__
    printf("Open /dev/cuaUx on OpenBSD\n\r");
    fd = open(pport_default_openbsd, O_RDWR | O_NONBLOCK);

    // On OpenBSD, change from default serial line speed to 115kB
    // Code issued by ChatGPT
    {
      struct termios tty;
      // Configure serial port
      memset(&tty, 0, sizeof(tty));
      int serial_port = fd;

      if (tcgetattr(serial_port, &tty) != 0) {
          perror("Error reading serial port attributes");
          close(serial_port);
      }

      // Set baud rate
      cfsetospeed(&tty, B115200);
      cfsetispeed(&tty, B115200);

      // 8N1 mode
      tty.c_cflag &= ~PARENB;
      tty.c_cflag &= ~CSTOPB;
      tty.c_cflag &= ~CSIZE;
      tty.c_cflag |= CS8;

      // Disable hardware flow control
      tty.c_cflag &= ~CRTSCTS;

      // Disable software flow control
      tty.c_iflag &= ~(IXON | IXOFF | IXANY);

      // Raw input
      tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

      // Raw output
      tty.c_oflag &= ~OPOST;

      // Set the new attributes
      if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
          perror("Error setting serial port attributes");
          close(serial_port);
      }
    }
    #endif
    if(fd>0) printf("Open successfully\n");
    else printf("Failed to open device\n");
    //memset(buf, 0, sizeof(buf));
    (void)pport_default_linux;
    (void)pport_default_openbsd;
}

// TODO Refactor, Generalize, defines into state machine, remove defines from main and cgl
#define MOT_CMD_RUN       1
#define MOT_CMD_SLEEP     2
#define MOT_CMD_PAUSE     3
#define MOT_CMD_MODE_AUTO 4
#define MOT_CMD_MODE_MAN  5
#define MOT_CMD_SPD_PLUS  6
#define MOT_CMD_SPD_MINUS 7
#define MOT_CMD_DIR_CCW   8
#define MOT_CMD_DIR_CW    9
#define MOT_CMD_GO_SLOW   10
#define MOT_CMD_HOLD_POS  11

void csl::Serial::non_blocking_write(int vin) {
    int wr = 0;
    (void)wr;
    if (vin == MOT_CMD_RUN) wr = write(fd, "r", 1);
    else if (vin == MOT_CMD_SLEEP) wr = write(fd, "s", 1);
    else if (vin == MOT_CMD_PAUSE) wr = write(fd, "=", 1);
    else if (vin == MOT_CMD_MODE_AUTO) wr = write(fd, "a", 1);
    else if (vin == MOT_CMD_MODE_MAN) wr = write(fd, "m", 1);
    else if (vin == MOT_CMD_SPD_PLUS) wr = write(fd, "+", 1);
    else if (vin == MOT_CMD_SPD_MINUS) wr = write(fd, "-", 1);
    else if (vin == MOT_CMD_DIR_CCW) wr = write(fd, "<", 1);
    else if (vin == MOT_CMD_DIR_CW) wr = write(fd, ">", 1);
    else if (vin == MOT_CMD_GO_SLOW) wr = write(fd, "g", 1);
    else if (vin == MOT_CMD_HOLD_POS) wr = write(fd, "P", 1);
}

std::string csl::Serial::blocking_read() {
    if (fd>0) {
        char buf[512] = {0};
        int buf_s = 0;
        int nr {0};
        // while ((nr = read(fd, buf, sizeof(buf))) != -1 && nr != 0);
        do {
            nr = read(fd, buf, sizeof(buf));
            if (nr > 0) buf_s = nr;
        } while (nr != -1 && nr != 0);

        // printf("%s", buf);
        // fflush(stdout);
        if (buf_s > 0) {
          str_out = std::string(buf, buf_s);
          // cout << "str_out " << str_out << " buf_s " << buf_s << endl;
          // cout << str_out << flush;
          return str_out;
        } else {
          return "";
        }
    } else {
        printf("fd not open\r");
        return "";
    }
}

csl::Serial::~Serial() {if (fd) close(fd);}



// ************* SFML Drawables *************

/// private:
///
///     virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
///     {
///         // You can draw other high-level objects
///         target.draw(m_sprite, states);
///
///         // ... or use the low-level API
///         states.texture = &m_texture;
///         target.draw(m_vertices, states);
///
///         // ... or draw with OpenGL directly
///         glBegin(GL_QUADS);
///         ...
///         glEnd();
///     }
///
///     sf::Sprite m_sprite;
///     sf::Texture m_texture;
///     sf::VertexArray m_vertices;
