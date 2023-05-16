void record_game(int winner) {
    FILE* scoreboard_fp;
    int thepipe[2];
    char buf[BUFSIZ];

    if (pipe(thepipe) == -1) {
        perror("pipe");
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(1);
        // child process records the result
    case 0:
        close(thepipe[1]);
        read(thepipe[0], buf, BUFSIZ);

        scoreboard_fp = fopen("./bin/scoreboard", "a+");

        if (scoreboard_fp == NULL) {
            perror("scoreboard_fp fopen");
            exit(1);
        }

        fprintf(scoreboard_fp, "%s", buf);
        fflush(scoreboard_fp);

        exit(0);
        // parent process sends the result to the child process
    default:
        close(thepipe[0]);

        if (winner == ATTACKER) {
            sprintf(buf, "%s %s %s %ld\n", player_1, player_2, player_1, time(NULL));
        }
        else if (winner == DEFENDER) {
            sprintf(buf, "%s %s %s %ld\n", player_1, player_2, player_2, time(NULL));
        }

        if (write(thepipe[1], buf, strlen(buf)) != strlen(buf)) {
            perror("write");
            exit(1);
        }

        wait(NULL);

        printf("Record completed.\n");
        break;
    }
}