<?php

$port = "1234";
$sdp_file_name = "test_gen.sdp";
$curr_ip_file = "../iPod_Ping/current_ip.txt";
generate_sdp($sdp_file_name,$curr_ip_file,$port);
	
//génère le fichier SDP pour la lecture distante avec VLC sur l'ip contenue dans le fichier current_ip.txt du serveur et au port 1234,
//La vidéo est au format h264
function generate_sdp($sdp_file_name,$curr_ip_file,$port){
	$target_ip = file_get_contents($curr_ip_file);
	unlink($sdp_file_name);
	$sdp = fopen($sdp_file_name, 'a+');
  	$content = "v=0\r\no=- 0 0 IN IP4 127.0.0.1\r\ns=No Name\r\nc=IN IP4 ".$target_ip."\r\nt=0 0\r\na=tool:libavformat 56.40.101\r\nm=video ".$port." RTP/AVP 96\r\na=rtpmap:96 H264/90000\r\na=fmtp:96 packetization-mode=1\r\n";
	fputs($sdp, $content);
	fclose($sdp);
	echo "done :".$target_ip;
}


?>

