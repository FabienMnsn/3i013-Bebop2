<?php

if(isset($_GET["action"])){
	$action = htmlspecialchars($_GET["action"]);
	switch($action){
		case "ping" ://on répond au ping de l'iPod
	
			$ip = $_SERVER['REMOTE_ADDR'];//ip du client

			//maj des logs :
			$monfichier = fopen("logs.txt", 'a');
			$date = date('m/d/Y h:i:s a', time());
		  	$content = $ip." [ ".$date." ]\r\n";
			fputs($monfichier, $content);
			fclose($monfichier);

			echo "OK";
			break; 

		case "connect":
			$ip = $_SERVER['REMOTE_ADDR'];//ip du client

			//maj de l'ip courrante
			unlink("current_ip.txt");
			$ipFile = fopen("current_ip.txt", 'a');//on enregistre l'ip du client
			fputs($ipFile, $ip);
			fclose($ipFile);

			echo "Connected";
			break;

		case "reset"://initialisation des fichiers pour le vol du drone (doit être fait avant le lancement du programme du drone ou après)
			//on flush les fichiers de signalement (start/stop):
			unlink("stop.txt");
			$stopFile = fopen("stop.txt", 'a');
			fclose($stopFile);
			unlink("start.txt");
			$startFile = fopen("start.txt", 'a');
			fclose($startFile);
			echo "reset done";
			break;

		case "start"://démarrage du plan de vol du drone

			//on écris dans le fichier pour signaler que le drone doit décoller
			unlink("start.txt");
			$startFile = fopen("start.txt", 'a');
			fputs($startFile, "s");
			fclose($startFile);

			echo "started"; 
			break;

		case "stop": //arrêt d'urgence du drone

			//on écris dans le fichier pour signaler que le drone doit décoller
			unlink("stop.txt");
			$stopFile = fopen("stop.txt", 'a');
			fputs($stopFile, "s");
			fclose($stopFile);

			echo "stoped"; 
			break;

		default:
			echo "invalid parameters";
			break;
	}

	
}
else{
	echo "invalid parameters";
}


?>
