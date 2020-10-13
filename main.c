/*
    Copyright (c) 2020 medit
    Distributed under the MIT License. See ./LICENSE.
*/

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){
	int life = 1, score = 1, anstime = 1;
	int i, j, barsnow, barsold, ans, input, cont;
	char c;
	struct timespec ts;
	struct timeval tstart, tnow;
	long remainus;

	while((c = getopt(argc, argv, "hl:t:")) != -1){
		switch(c){
			case 'l': life = atoi(optarg); break;
			case 't': anstime = atoi(optarg); break;
			case 'h':
			default:
				fprintf(stderr, "\
\n\
FizzBuzz Game v1.10 - 2020-10-14\n\
Copyright (c) 2020 medit\n\
This software is distributed under the MIT License.\n\
\n\
  Usage: %s [-h] [-l <lives>] [-t <answer_time>]\n\
\n\
      -l    number of lives (positive integer, 1 to 99)\n\
      -t    length of answer time (in seconds, 1 to 9)\n\
      -h    show this help\n\
\n\
", argv[0]);
				return 1;
		}
	}
	if(life <= 0 || life >= 99){
		fprintf(stderr, "error: wrong number of lives (must be 1 to 99)\n");
		return 1;
	}
	if(anstime <= 0 || anstime >= 10){
		fprintf(stderr, "error: wrong answer time (must be 1 to 9 seconds)\n");
		return 1;
	}
	system("stty -echo -icanon");
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);
	printf("\
\n\
FizzBuzz Game\n\
\n\
----------------------- How to play -----------------------\n\
Press [Left]  key to say \"Fizz\".\n\
Press [Right] key to say \"Buzz\".\n\
Press [Down]  key to say the displayed number.\n\
\n\
You must answer in %d sec. You lose one life when you make\n\
a mistake or don't answer in time.\n\
------------------------------------------------------------\n\
", anstime);

	/* initialize drawing area */
	printf("Life:");
	for(int i = 0; i < life; i++) printf(" *");
	printf("\n\n              ->  \n\n");
	printf("( %4d ms )[", anstime * 1000);
	for(i = 0; i < 100; i++) putchar('|');
	printf("]\n* Press [Space] key to start *\n");
	while(getchar() != ' ');

	/* main loop */
	while(1){
		/* redraw */
		printf("\e[6A\e[2KLife:");
		for(int i = 0; i < life; i++) printf(" *");
		printf("\n\e[2K\n\e[2K    %-8d  ->  \n\e[2K\n", score);
		printf("(      ms )[");
		for(i = 0; i < 100; i++) putchar('|');
		printf("]\n[Left] Fizz     [Down] %-8d [Right] Buzz\n", score);

		/* prepare flags */
		gettimeofday(&tstart, NULL);
		ans = 0;
		if(score % 3 == 0) ans += 1;
		if(score % 5 == 0) ans += 2;
		barsnow = 100;
		input = -1;
		cont = 0;
		/* input-waiting loop */
		while(1){
			gettimeofday(&tnow, NULL);
			remainus = anstime * 1000000L;
			remainus -= (tnow.tv_sec - tstart.tv_sec) * 1000000L + (tnow.tv_usec - tstart.tv_usec);
			if(remainus < 0) remainus = 0;
			/* input-processing loop */
			while((c = getchar()) != EOF){
				switch(c){
					case 'D': input = 1; break;
					case 'B': input = 0; break;
					case 'C': input = 2; break;
				}
				if(input != -1){
					printf("\e[4A\e[18C");
					if(cont) printf("\e[4C");
					switch(input){
						case 1: printf("Fizz"); break;
						case 0: printf("%d", score); break;
						case 2: printf("Buzz"); break;
					}
					printf("\n\n\n\n");
					if(cont){
						if(input == 2) input = 3;
						cont = 0;
						break;
					}else if(ans == 3 && input == 1){
						cont = 1;
						input = -1;
					}else break;
				}
			}
			barsold = barsnow;
			barsnow = (remainus + anstime * 10000 - 1) / (anstime * 10000);
			printf("\e[2A\e[2C%4ld\e[%dC", (remainus + 999) / 1000, 6 + barsnow);
			for(i = barsnow; i < barsold; i++) putchar(' ');
			printf("\n\n");
			if(input != -1 || remainus <= 0) break;
			ts.tv_sec = 0;
			ts.tv_nsec = 10000000;
			nanosleep(&ts, NULL);
		} /* input is always -1 here when cont is 1 */
		if(cont) input = 1;
		if(input == ans){
			ts.tv_sec = 0;
			ts.tv_nsec = 250000000;
			nanosleep(&ts, NULL);
			while(getchar() != EOF);
		}else{
			printf("\e[3A");
			if(input == -1) printf("Time over!");
			else{
				printf("Wrong! The answer is '");
				if(ans & 1) printf("Fizz");
				if(ans & 2) printf("Buzz");
				if(ans == 0) printf("%d", score);
				printf("'.");
			}
			printf("\n\n\n");
			life--;
			if(life <= 0) break;
			printf("\e[A\e[2K* Press [Space] key to continue *\n");
			while(getchar() != ' ');
		}
		score++;
		if(score > 99999999) break; /* limit score to 8 digits */
	}
	printf("\nGAME OVER\nYour score was %d.\n\n", score - 1);
	system("stty echo icanon");
	return 0;
}
