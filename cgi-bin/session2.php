<?php
    session_id('mySessionID');
    session_start();

    if(isset($_SESSION['name'])) {
        echo "Hello " . $_SESSION['name'] . "<br>";
    } else {
        ech
?>
