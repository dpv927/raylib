use raylib::prelude::*;
use rand_distr::Distribution;
use tetromino::{Tetronimo, Direction};
use frame::Frame;

mod tetromino;
mod frame;

const TILE_SIZE: i32 = 70;

fn main() {
     let (mut rl, thread) = raylib::init()
        .size(0, 0)
        .title("Map Generation")
        .log_level(TraceLogLevel::LOG_ERROR)
        .build();
    rl.set_target_fps(10);

    {
        let monitor_width  = window::get_monitor_width(window::get_current_monitor());
        let monitor_height = window::get_monitor_height(window::get_current_monitor());
        /* Main frame width (10 + 2) + space (1) + next frame width (4 + 2) */
        let window_width  = 19 * TILE_SIZE;
        /* Main frame height (20 + 2) */
        let window_height = 22 * TILE_SIZE;

        rl.set_window_position((monitor_width - window_width)/2, (monitor_height - window_height)/2);
        rl.set_window_size(window_width, window_height);
    }

    let mut rng = rand::thread_rng();
    let dist = rand_distr::Uniform::new(0,6);

    // Frames (Places to show tetrominoes and blocks)
    // In the main_frame we show the game, and in the next_frame e show
    // the next tetromino to pick.
    let main_frame = Frame::new(20, 10, TILE_SIZE, 0, 3, 0);
    let next_frame = Frame::new(4,  4,  TILE_SIZE, 13 * TILE_SIZE, 0, 0);

    // Current tetromino and the next one.
    let mut actual = Tetronimo::new(dist.sample(&mut rng), main_frame.def_x, main_frame.def_y);
    let next   = Tetronimo::new(dist.sample(&mut rng), next_frame.def_x, next_frame.def_y);

    while !rl.window_should_close() {

        // Move the tetromino vertically.
        if rl.is_key_down(KeyboardKey::KEY_A) {
            actual.move_to_side(Direction::Left);

        } else if rl.is_key_down(KeyboardKey::KEY_D) {
            actual.move_to_side(Direction::Right);
        }

        // Rotate the tetromino if possible
        if rl.is_key_pressed(KeyboardKey::KEY_UP) {
            actual.rotate();
        }

        let mut d = rl.begin_drawing(&thread);
        d.clear_background(Color::BLACK);

        main_frame.draw_tetronimo(&actual, &mut d);
        next_frame.draw_tetronimo(&next, &mut  d);
        main_frame.draw_self(&mut d);
        next_frame.draw_self(&mut d);
    }
}
