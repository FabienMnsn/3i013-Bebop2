<?php
if(isset($_POST["content"])&&isset($_POST["name"])){//on enregistre le plan de vol demandé
	$content = htmlspecialchars($_POST["content"]);
	$name = htmlspecialchars($_POST["name"]);
	echo $content;
	echo $name;
	unlink($name);//on efface l'ancien fichier
	$monfichier = fopen($name, 'a');
	fputs($monfichier, $content);
	fclose($monfichier);
}
else{//on redirige sur l'appli de saisie du plan de vol
	$host  = $_SERVER['HTTP_HOST'];
	$uri   = rtrim(dirname($_SERVER['PHP_SELF']), '/\\');
	$extra = 'saisie_plan_de_vol.html';
	header("Location: http://$host$uri/$extra");
}


?>
