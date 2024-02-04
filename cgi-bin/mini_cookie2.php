<?php
// Check if the cookie is set
if(isset($_COOKIE["username"])) {
    echo "Welcome " . $_COOKIE["username"] . "!";
} else {
    echo "Cookie not set!";
}
?>
