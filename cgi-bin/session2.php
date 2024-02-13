<?php
    session_id('mySessionID');
    session_start();

    if(isset($_SESSION['name'])) {
        echo $_SESSION['name'] . " Still HeRe!<br>";
    } else {
        echo "Session variable 'name' not set.";
    }
?>
