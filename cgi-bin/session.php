<?php
session_id('mySessionID');
  session_start();

  // echo session_id();

  $_SESSION['name'] = "Mourad";

  echo "Hello " . $_SESSION['name'] . "<br>";

  echo '<a href="session2.php">Count views</a>';
?>

