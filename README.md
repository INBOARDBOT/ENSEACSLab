# ENSEACSLab
Git repo of CS lab report by Julien BURGER, Aleksandar DJOKIC 


Dans ce TP, l'objectif est de créer un micro Shell qui servira de lancer plusieurs commandes.

1. Au départ, on veut afficher un message d'accueil lorsque l'on entre dans le Shell, pour ce faire il suffit juste d'utiliser la commande write()

2. Ensuite, il faut que le système puisse lire et exécuter la commande que l'on saisie dans le terminale. Pour ce faire, on utilise les commandes stdin et stdout qui établi le lien entre la commande à exécuter et l'affichage du résultat. Typiquement, on peut lancer la commande Linux "Fortune".

3. On souhaiterait aussi sortir du Shell (et non du bash), on créer donc la commande "exit" qui permet d'arriver à cette fin. Techniquement, la commande "exit" arrête la boucle  ShellRunning.

4. Pour afficher le signal de la commande précédente dans le prompt, on utilise la fonction ShellnamePrompt qui affiche le code de sortie si la dernière commande s'est terminé normalement et affiche un signal si elle a été interrompu.

5. La mesure du temps de l'exécution s'effectue à l'aide de la commande clock_gettime. On utilise un fork() dans la fonction executeExternalCommand() de tel sorte à ce que l'enfant exécute la commande (execlp) et que le parent attend la fin de l'enfant et calcule le temps écoulé en ms. Si fork() échoue, alors on affiche le code  de sortie ainsi que le temps initiale.

6. Cette fois ci, on veut exécuter une commande complexe, autrement dit que le Shell exécute une commande en prenant un argument. On récupère tout les tokens, le 1er tokens correspond à la commande, ceux qui suivent sont les arguments.

7) Pour gérer les redirections vers stdin et stdout avec '<' et '>' , On utilise la fonction parseShellUserInput qui permet de découper la ligne en tokens et détecte si il y a une redirections vers l'entrée ('>' ici) ou la sortie ('<' ici) dans le token. En fonction du résultat, on redirige vers stdin ou stdout.

8) L'exécution de commandes chaînées avec une pipe s'effectue avec RedirectCmd. Le programme cherche le caractère "|" et fait une redirection vers la gestion des pipes si il est présent (au cas contraire il effectue une exécution classique). Ensuite, on sépare la commande en deux parties et on exécute les deux processus connectés par pipe.

9) Dans cette dernière question, on souhaite exécuter des commandes en arrière plan. On doit d'abord les détecter avec la fonction findbckgrndProcessCmd (vérifie si le dernier argument est '&') puis on exécute la commande en arrière plan dans ExecuteExternalCommand. On y ajoute ensuite un Job avec actualizeProcessTable et on surveille ces derniers grâce à checkBackgroundJobs jusqu'à ce qu'il soit terminer (auquel cas, on affiche ces derniers).