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


#ifndef CSL_H
#define CSL_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <memory>
#include <vector>

namespace csl {

class Push_button: public sf::Drawable
{
    sf::Texture tex_on;
    sf::Texture tex_off;
    int state {0};

    public:

        sf::Sprite sprite;

        Push_button();
        Push_button(std::string, std::string); // Tex on/off
        Push_button(std::string, std::string, sf::Vector2<float>, float); // Tex on/off, pos, scale (xy)
        ~Push_button();

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        void load_tex_on(std::string);
        void load_tex_off(std::string);
        void load_tex_on_off(std::string, std::string);

        void set_on();
        void set_off();
        void set_blink();
        void set_pulse_on();
        void set_pulse_off();
        void set_toggle();

        void set_test();
        void set_normal();

        void set_lock();
        void set_unlock();

        void test_light_act();
        void test_light_dis();

        int is_on();
        int is_off();
        int is_pressed(const sf::RenderWindow & win);
};


class Seven_seg_digit {
    sf::Texture dig_tex[11];
public:
    sf::Sprite dig_sprite;
    Seven_seg_digit();
    void set_dig(int); /// Set the display value 0-9
    void set_dot(int); /// TODO Set Dot
    void draw(sf::RenderWindow *win); /// Standard SFML Draw
    int operator = (int); /// Another way to set digit value 0-9
};

class Seven_seg_display {
    std::vector<std::shared_ptr<Seven_seg_digit>> seven_seg_digits;
    const int h_spacing {30}; /// dx pix
public:
    Seven_seg_display (unsigned int);
    void draw(sf::RenderWindow *win);
    void set_digit(int, int); /// Digit, Value
    void set_position (int x, int y); /// x, y
    void set_scale (const float scalex, const float scaley); /// Scale x,y
    void set_scale (const float scale); /// Scale
    int operator = (int); /// Set display value
};


// ************* Serial Communications *************

/*
 * TODO
 * - Inherit from Communications class
 * - Define communication invariants
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// OpenBSD:
#include <termios.h>

class Serial {
private:
    int fd = 0; // file descriptor
    const char *pport_default_linux = "/dev/ttyACM2";
    const char *pport_default_openbsd = "/dev/cuaU0";
    std::string str_out;
public:
    Serial();
    void non_blocking_write(int vin);
    std::string blocking_read();
    ~Serial();
};


}

#endif // CSL_H
