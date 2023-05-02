// end the game, if want, record the result of the game
void end_game(int winner) {
    int option;

  // the game finish, so ignore the signal
    signal(SIGINT, SIG_IGN);
    signal(SIGALRM, SIG_IGN);

    if (winner == ATTACKER)
        printf("ATTACKER is winner!\n");
    else if (winner == DEFENDER)
        printf("DEFENDER is winner!\n");

    printf("Do you want to record the result of this game at the scoreboard?\n");
    printf("1) yes\n");
    printf("2) no\n");
    printf(": ");
    scanf("%d", &option);

    if (option == 2) {
        printf("Good Game!\n");
        exit(0);
        }
    puts("-------------------------------------");
    puts("| Input your name within 3 letters. |");
    puts("-------------------------------------");

    printf("Player 1 (ATTACKER): ");
    scanf("%s", player_1);

    printf("Player 2 (DEFENDER): ");
    scanf("%s", player_2);
  
    record_game(winner);
    printf("Good Game!\n");
    exit(0);
}