<?php

     $mysql_host = "localhost";

     $mysql_user = "ubuntu";

     $mysql_password = "didtjdah";

     $mysql_db = "iotdb";

 

     $conn = mysqli_connect($mysql_host, $mysql_user, $mysql_password, $mysql_db);

 

     if (!$conn) {

         die("Database Connect Error: " . mysqli_connect_error());

     }

     $android = strpos($_SERVER['HTTP_USER_AGENT'], "Android");

 

     if ((($_SERVER['REQUEST_METHOD'] == 'POST') && isset($_POST['submit'])) || $android)

     {

         $start_date = trim($_POST['start']);

         $end_date = trim($_POST['end']);

         $sql = "SELECT * FROM feed_info  WHERE date >='$start_date' AND date <='$end_date'";



         $result = mysqli_query($conn, $sql);

         $i = 1;

 

         if (mysqli_num_rows($result) >= 1 ) {

             while( $i <= mysqli_num_rows($result) ){

                 $i++;

                 $member = mysqli_fetch_assoc($result);

                 echo "NAME:" . $member['name'] . ",DATA:".$member['date'].",TIME:".$member['time'].",FOOD:".$member['food']."\n";

             }

         } else {

             echo "No Data.";

              mysqli_close($conn);

              exit;

          }

      } else {

          echo "No Android Device Detected.";

    }

      mysqli_close($conn);

 ?>


