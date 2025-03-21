#include <stdio.h>
#include <term.h>
#include <sys/time.h>

#define RAND_MAX 32767
unsigned long next = 0;

int rand() {
	if (next == 0) {
		next = _time();
	}
    next = next * 1103515245 + 12345;
    return (uint32_t) (next / 65536) % RAND_MAX + 1; 
}

uint8_t buffer[1000][1000] = {0};
uint8_t resets[1000] = {0};
uint64_t num_frames_rendered = 0;

uint64_t num_frames_rendered_in_10_seconds = 0;
long long start_time = 0;
int fps = 0;

void frame() {
	if (start_time == 0) {
		start_time = _time();
	}

	struct point_t screen_size = get_screen_size();

	int rows = screen_size.y / 16 - 2;
	int cols = screen_size.x / 8 - 1;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (i == 0) {
				buffer[i][j] = (buffer[i][j] + 1) % (resets[j] + 2);
				
				if(buffer[i][j] == 0) {
					resets[j] = (rand() % rows);
				}

			} else {
				if(buffer[i - 1][j] != 0) {
					if(buffer[i - 1][j] - 1 > 0) {
						buffer[i][j] = buffer[i - 1][j] - 1;
					} else {
						buffer[i][j] = 0;
					}
				} else {
					buffer[i][j] = 0;
				}
			}
		}
	}

	for (int i = 0; i < rows; i++) {
		set_cursor((struct point_t) { 0, i * 16 });
		set_color(0);
		clear_line();

		for (int j = 0; j < cols; j++) {
			
			if (buffer[i][j]) {
				set_cursor((struct point_t) { j * 8, i * 16 });
				set_color(0xff000000 | ((0xff - buffer[i][j]) << 12));
				putchar('X');
			}

		}
	}

	num_frames_rendered++;

	set_color(0);
	set_cursor((struct point_t) { 0, screen_size.y - 16 });
	clear_line();
	set_color(0xffffffff);
	printf("frames: %d (%d fps next update in %d s)", num_frames_rendered, fps, start_time + 10 - _time());

	if (start_time + 10 - _time() < 0) {
		start_time = 0;
		fps = num_frames_rendered_in_10_seconds / 30;
		num_frames_rendered_in_10_seconds = 0;
	} else {
		num_frames_rendered_in_10_seconds++;
	}
}

int main() {
	while (1) {
		frame();
	}

	return 0;
}