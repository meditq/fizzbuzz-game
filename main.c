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
	if(argc >= 2) life = atoi(argv[1]);
	if(life <= 0){
		fprintf(stderr, "\
FizzBuzz Game v1.00 - 2020-10-12\n\
Copyright (c) 2020 medit\n\
\n\
    Usage: ./fbgame [lives]\n\
\n\
`lives` must be a positive integer.\n\
If not specified, 1 is used.\n\
");
		return 1;
	}
	system("stty -echo -icanon");
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);
	printf("\
*** FizzBuzz Game ***\n\
\n\
[How to play]\n\
Press [Left]  key to say \"Fizz\".\n\
Press [Right] key to say \"Buzz\".\n\
Press [Down]  key to say the displayed number.\n\
\n\
You must answer in %d sec.\n\
You lose one life when you make a mistake or don't answer in time.\n\
\n\
", anstime);
	printf("Life:");
	for(int i = 0; i < life; i++) printf(" *");
	printf("\n\n              ->  \n\n");
	printf("( %4d ms )[", anstime * 1000);
	for(i = 0; i < 100; i++) putchar('|');
	printf("]\n* Press [Space] key to start *\n");
	while(getchar() != ' ');
	printf("\e[6F");

	/* main loop */
	while(score <= 99999999){
		printf("\e[2KLife:");
		for(int i = 0; i < life; i++) printf(" *");
		printf("\n\e[2K\n    %-8d  ->  \n\e[2K\n", score);
		printf("(      ms )[");
		for(i = 0; i < 100; i++) putchar('|');
		printf("]\n[Left] Fizz     [Down] %-8d [Right] Buzz\n", score);
		gettimeofday(&tstart, NULL);
		ans = 0;
		if(score % 3 == 0) ans += 1;
		if(score % 5 == 0) ans += 2;
		barsnow = 100;
		input = -1;
		cont = 0;
		while(1){
			gettimeofday(&tnow, NULL);
			remainus = anstime * 1000000L;
			remainus -= (tnow.tv_sec - tstart.tv_sec) * 1000000L + (tnow.tv_usec - tstart.tv_usec);
			if(remainus < 0) remainus = 0;
			while((c = getchar()) != EOF){
				switch(c){
					case 'D': input = 1; break;
					case 'B': input = 0; break;
					case 'C': input = 2; break;
				}
				if(input != -1){
					printf("\e[4F\e[14C");
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
			printf("\e[2F\e[2C%4ld\e[%dC", (remainus + 999) / 1000, 6 + barsnow);
			for(i = barsnow; i < barsold; i++) putchar(' ');
			printf("\n\n");
			if(input != -1 || remainus <= 0) break;
			ts.tv_sec = 0;
			ts.tv_nsec = 10000000;
			nanosleep(&ts, NULL);
		}
		if(cont) input = 1;
		if(input == ans){
			ts.tv_sec = 0;
			ts.tv_nsec = 250000000;
			nanosleep(&ts, NULL);
		}else{
			printf("\e[3F");
			if(input == -1) printf("Time over!");
			else{
				printf("Wrong! The answer is '");
				if(ans & 1) printf("Fizz");
				if(ans & 2) printf("Buzz");
				if(ans == 0) printf("%d", score);
				printf("'.");
			}
			life--;
			printf("\n\n\n");
			if(life <= 0) break;
			printf("\e[F\e[2K* Press [Space] key to continue *\n");
			while(getchar() != ' ');
		}
		score++;
		printf("\e[6F");
	}
	printf("\nGAME OVER\nYour score was %d.\n\n", score - 1);
	system("stty echo icanon");
	return 0;
}