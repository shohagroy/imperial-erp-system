<?php
/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/

// Necessary File hook

//---------------------------------------------------------------------------------------

// Add, Edit, Delete, Get And Get Join 

//---------------------------------------------------------------------------------------
if(!function_exists('SMS_Insert')) {
function SMS_Insert($table_name, $data, $default_company=null, $show_sql=false){
    if($default_company != null)
        $sql0 = "INSERT INTO  ".MASTER_DB.".".MASTER_DB_PREF.$table_name." (";
    else
        $sql0 = "INSERT INTO ".TB_PREF.$table_name."(";
    $sql1 = " VALUES (";
    foreach($data as $key=>$value){
        $sql0 .= "`".$key."`,";
		if(is_array($value)) { 
			if($value[1] == 'date')				
				$sql1 .=  db_escape(date2sql($value[0])).",";
			if($value[1] == 'float')
				$sql1 .= $value.",";
		}else 
			$sql1 .= db_escape($value).",";
    } 
    $sql0 = substr($sql0, 0, -1).")";
    $sql1 = substr($sql1, 0, -1).")";
	//display_notification($sql0.$sql1);
    if($show_sql)
        display_error($sql0.$sql1);
    db_query($sql0.$sql1, "Could not insert data to table {$table_name}");
	return  db_insert_id();
}
}

if(!function_exists('SMS_Update')) {
function SMS_Update($table_name, $primary_key, $data, $default_company=null, $show_query=false){

    if(db_Has_Data_on_Table($table_name, $primary_key, $default_company, $show_query)){
        if($default_company != null)
            $sql0 = "UPDATE ".MASTER_DB.".".MASTER_DB_PREF.$table_name." SET";
        else
            $sql0 = "UPDATE ".TB_PREF.$table_name." SET ";
        foreach($data as $key=>$value){
            if(is_array($value)) { 
                if($value[1] == 'date')             
                    $sql0 .= "`".$key."` = ". kv_db_escape(date2sql($value[0])).",";
                if($value[1] == 'float')
                    $sql0 .= "`".$key."` = ". $value.",";
                if($value[1] == 'noesc')
                    $sql0 .= "`".$key."` = '". $value[0]."',";
            }else {
                $sql0 .= "`".$key."` = ".kv_db_escape($value).",";
            }
        }
        $sql0 = substr($sql0, 0, -1);
        $sql0 .= " where 1=1";
        foreach($primary_key as $key=>$value){
            if(is_array($value)) { 
                if(isset($value[2]))
                    $operator = $value[2];
                else
                    $operator = '=';

                if($value[1] == 'date')             
                    $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape(date2sql($value[0]));
                elseif($value[1] == 'float')
                    $sql0 .= " AND ".$key." ".$operator." ". $value;
                else
                    $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape($value[0]);
            }else{
                if(is_numeric($value)){
                    $sql0 .=" AND ". $key." = ".$value;
                }else
                   $sql0 .= " AND ".$key." = ".kv_db_escape($value);
            }
        }
	if($show_query)
		display_error($sql0);

       return  db_query($sql0, "Could not update data on table {$table_name}");
    }else{
        foreach($primary_key as $key => $value){
            if($key != 'id')
                $data[$key] = $value;
        }
         return SMS_Insert($table_name, $data, $default_company, $show_query);
    }     
}
}

if(!function_exists('db_Has_Data_on_Table')) {
    function db_Has_Data_on_Table($table_name, $primary_key =false, $default_company = null, $show_query=false){    
        if($default_company != null)
            $sql = "SELECT COUNT(*) FROM ".MASTER_DB.".".MASTER_DB_PREF.$table_name." WHERE 1=1";
        else
            $sql = "SELECT COUNT(*) FROM ".TB_PREF.$table_name." WHERE 1=1";
        if($primary_key){
            foreach($primary_key as $key=>$value){
                if(is_array($value)) { 
                    if($value[1] == 'date')             
                        $sql .= " AND ".$key." = ". kv_db_escape(date2sql($value[0])).",";
                    if($value[1] == 'float')
                        $sql .= " AND ".$key." = ". $value.",";
                }else{
                    if(is_numeric($value)){
                        $sql .=" AND ". $key." = ".$value;
                    }else
                       $sql .= " AND ".$key." = ".kv_db_escape($value);
                }
            }
        }
        if($show_query)
            display_error($sql0);
        return  check_empty_result($sql);
    }
}

if(!function_exists('kv_db_escape')) {
    function kv_db_escape($value = "", $nullify = false){
        global $db;
        
        $value = @html_entity_decode($value, ENT_QUOTES, $_SESSION['language']->encoding);
        $value = html_specials_encode($value);

        //reset default if second parameter is skipped
        $nullify = ($nullify === null) ? (false) : ($nullify);

        //check for null/unset/empty strings
        if ((!isset($value)) || (is_null($value)) || ($value === "")) {
            $value = ($nullify) ? ("NULL") : ("''");
        } else {
            if (is_string($value)) {
                $value = "'" . mysqli_real_escape_string($db, $value) . "'";
                //value is a string and should be quoted; 
            } else if (!is_numeric($value)) {
                //value is not a string nor numeric
                display_error("ERROR: incorrect data type send to sql query");
                echo '<br><br>';
                exit();
            }
        }
        if(strpos($value, '&quot;')){
           $value =str_replace("&quot;", '"', $value);
        }
        return $value;
    }
}

if(!function_exists('SMS_GetDataJoinRow')) {
	function SMS_GetDataJoinRow($main_table, $joins, $columns = array(), $conditions=null, $order_by= null, $group_by = null){
		return SMS_GetDataJoin($main_table, $joins, $columns, $conditions, $order_by, $group_by, true);
	}
}

if(!function_exists('SMS_Delete')) {
function SMS_Delete($table_name, $conditions, $default_company=null){
    if($default_company != null)
        $sql0 = "DELETE FROM ".MASTER_DB.".".MASTER_DB_PREF.$table_name." WHERE 1=1";
    else
        $sql0 = "DELETE FROM ".TB_PREF.$table_name." WHERE 1=1";
    foreach ($conditions as $key=>$value) {
        $sql0 .= " AND ".$key."=".kv_db_escape($value);
    }
    $result = db_query($sql0, "Could not delete data from {$table_name}");
    return $result;
}
}

if(!function_exists('GetQueryResult')){
	function GetQueryResult($sql, $res=false){
		$result = db_query($sql, "Can't get results");
		if(db_num_rows($result) > 0 ){
			if($res == 's'){
				$row = db_fetch($result);
				return $row[0];
			} elseif($res == 'r'){
				return db_fetch($result);
			}	else{
				$final = array();
				while($row = db_fetch($result))
					$final[] = $row;
				return $final;
			}
		} else
			return false;
	}
}
if(!function_exists('SMS_GetDataJoin')) {
    function SMS_GetDataJoin($main_table, $joins, $columns=array(), $conditions=null, $order_by = null, $group_by = null, $single=false, $sql=false, $default_company=null){
        $sql0 = "SELECT ";

        if($default_company){
            $tbpref = MASTER_DB.".".MASTER_DB_PREF;
        } else 
            $tbpref = TB_PREF;
        if(count($columns)>0){
            foreach ($columns as $value) {
                $sql0 .= $value.",";
            }
            $sql0 = substr($sql0, 0, -1);
        }else{
            $sql0 .= " *";
        }
        $sql0 .= " FROM ".$tbpref."{$main_table} ";
        // $sql0 .= " FROM ".TB_PREF."{$main_table} ";
        foreach ($joins as $value) {
            if(isset($value['join'])){
                $sql0 .= " {$value['join']} JOIN ".$tbpref.$value['table_name'].' ON '.$value['conditions'];
            }else{
                $sql0 .= " INNER JOIN ".$tbpref.$value['table_name'].' ON '.$value['conditions'];
            }
        }
        $sql0 .= " WHERE 1=1 ";
        if($conditions != null) {
            foreach($conditions as $key=>$value){

                if(is_array($value)) { 
                    if(isset($value[2]))
                        $operator = $value[2];
                    else
                        $operator = '=';

                    if($value[1] == 'date')             
                        $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape(date2sql($value[0]));
                    elseif($value[1] == 'float')
                        $sql0 .= " AND ".$key." ".$operator." ". $value;
                    else
                        $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape($value[0]);
                }else
                    $sql0 .= " AND {$key} = ".kv_db_escape($value);
            }
        }

        if($group_by != null) {
                $sql0 .=" GROUP BY ";
                foreach($group_by as $value){
                    $sql0 .= " ${value}";
                }
        }

        //echo $sql0;
        if($order_by != null) {
            $sql0 .=" ORDER BY ";
            foreach($order_by as $key=>$value){
                $sql0 .= " {$key} ${value}";
            }
        }
        if($single)
    		$sql0 .= ' LIMIT 1'; 
    	if($sql)
    		return $sql0;
        $result = db_query($sql0, "Could not get data!");
        $data = array();
        if(db_num_rows($result) > 0 ) {
            while($row = db_fetch($result)) {
                $data[] = $row;
            }
            if($single){               
                return $data[0];
            }
            else
                return $data;
        } else 
            return false;
    }
}

if(!function_exists('SMS_GetAll')) {
    function SMS_GetAll($table_name, $conditions = null, $order_by = null, $group_by = null, $default_company=null, $show_query=false, $return_sql=false){
        if($default_company != null)
            $sql0 = "SELECT * FROM ".MASTER_DB.".".MASTER_DB_PREF.$table_name." WHERE 1=1";
        else
            $sql0 = "SELECT * FROM ".TB_PREF.$table_name." WHERE 1=1";
        if($conditions != null) {
    		foreach($conditions as $key=>$value){
                if(is_array($value)) { 
                    if(isset($value[2]))
                        $operator = $value[2];
                    else
                        $operator = '=';

                    if($value[1] == 'date')             
                        $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape(date2sql($value[0]));
                    elseif($value[1] == 'float')
                        $sql0 .= " AND ".$key." ".$operator." ". $value;
                    else
                        $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape($value[0]);
                }else
    			 $sql0 .= " AND {$key} = ".kv_db_escape($value);
    		}
        }
       
        if($group_by != null) {
            $sql0 .=" GROUP BY ";
            foreach($group_by as $value){
                $sql0 .= " ${value}";
            }
        }
        if($order_by != null) {
            $sql0 .=" ORDER BY ";
            foreach($order_by as $key=>$value){
                $sql0 .= " {$key} ${value}";
            }
        }
        if($return_sql)
            return $sql0;
        $result = db_query($sql0, "Could not get data from {$table_name}", $show_query);
        $data = array();
        while($row = db_fetch_assoc($result)) {
            $data[] = $row;
        }
        return $data;
    }
}

if(!function_exists('SMS_GetRow')) {
    function SMS_GetRow($table_name, $conditions = null, $order_by = null,  $group_by= null,  $default_company=null){
        if($default_company != null)
            $sql0 = "SELECT * FROM ".MASTER_DB.".".$default_company.$table_name." WHERE 1=1";
        else
            $sql0 = "SELECT * FROM ".TB_PREF.$table_name." WHERE 1=1";
        if($conditions != null) {
            foreach($conditions as $key=>$value){
                if(is_array($value)) { 
                    $sql0 .= " AND ".$key." ".$value[1]." ". kv_db_escape($value[0]);
                } else
                    $sql0 .= " AND {$key} = '${value}'";
            }
        }

        if($group_by != null) {
                $sql0 .=" GROUP BY ";
                foreach($group_by as $value){
                    $sql0 .= " ${value}";
                }
        }

        if($order_by != null) {
            $sql0 .=" ORDER BY ";
            foreach($order_by as $key=>$value){
                $sql0 .= " {$key} ${value}";
            }
        }
    	
    	$sql0 .= ' LIMIT 1'; 
        $result = db_query($sql0, "Could not get data from {$table_name}");
    	$data = db_fetch($result);
        
        return $data;
    }
}

if(!function_exists('SMS_GetDataFilter')) { 
    function SMS_GetDataFilter($table_name, $columns= array(), $conditions=null, $order_by = null, $group_by= null, $default_company=null){
        $sql0 = "SELECT ";
        if(count($columns)>0){
            foreach ($columns as $value) {
                $sql0 .= $value.",";
            }
        }else{
            $sql0 .= " * ";
        }
        // remove the last character ','
        $sql0 = substr($sql0, 0, -1);
       

        if($default_company != null)
             $sql0 .= " FROM ".MASTER_DB.".".$default_company.$table_name." WHERE 1=1"; 
        else
             $sql0 .= " FROM ".TB_PREF.$table_name." WHERE 1=1";
         if(!empty($conditions)){
            foreach($conditions as $key=>$value){
                if(is_array($value)) { 
                    if(isset($value[2]))
                        $operator = $value[2];
                    else
                        $operator = '=';

                    if($value[1] == 'date')             
                        $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape(date2sql($value[0]));
                    elseif($value[1] == 'float')
                        $sql0 .= " AND ".$key." ".$operator." ". $value;
                    else
                        $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape($value[0]);
                } else 
                    $sql0 .= " AND {$key} = ${value}";
            }
        }


        if($group_by != null) {
                $sql0 .=" GROUP BY ";
                foreach($group_by as $value){
                    $sql0 .= " ${value}";
                }
        }
        if($order_by != null) {
            $sql0 .=" ORDER BY ";
            foreach($order_by as $key=>$value){
                $sql0 .= " {$key} ${value}";
            }
        }

        $result = db_query($sql0, "Could not get data from {$table_name}");
        $data = array();
        while($row = $result->fetch_assoc()) {
            $data[] = $row;
        }
        return $data;
    }
}

if(!function_exists('SMS_GetSingleValue')) {
    function SMS_GetSingleValue($tablename, $column_single, $conditions=null, $order_by = null, $group_by= null, $default_company=null, $show_sql=false){
    	if($default_company != null)
            $sql0 = "SELECT ".$column_single." FROM ".MASTER_DB.".".$default_company.$tablename." WHERE 1=1";
        else
            $sql0 = "SELECT ".$column_single." FROM ".TB_PREF.$tablename." WHERE 1=1";
        if($conditions){
            foreach($conditions as $key=>$value){
                if(is_array($value)) { 
                    if(isset($value[2]))
                        $operator = $value[2];
                    else
                        $operator = '=';

                    if($value[1] == 'date')             
                        $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape(date2sql($value[0]));
                    elseif($value[1] == 'float')
                        $sql0 .= " AND ".$key." ".$operator." ". $value;
                    else
                        $sql0 .= " AND ".$key." ".$operator." ". kv_db_escape($value[0]);
                }else
                    $sql0 .= " AND {$key} = '${value}'";
            }
        }
        
        if($group_by != null) {
    			$sql0 .=" GROUP BY ";
    			foreach($group_by as $value){
    				$sql0 .= " ${value}";
    			}
    		}

        if($order_by != null) {
            $sql0 .=" ORDER BY ";
            foreach($order_by as $key=>$value){
                $sql0 .= " {$key} ${value}";
            }
        }
        if($show_sql)
           return $sql0;
        $result = db_query($sql0, "could not get sales type");
        $row = db_fetch_row($result);

    		$data = @unserialize(base64_decode($row[0]));
    		if ($data !== false) {
    		    return unserialize(base64_decode($row[0]));
    		}else
    			return $row[0];
    }
}
if(!function_exists('CheckEmptyResult')) {
    function CheckEmptyResult($table, $msg, $condition=null) {
        $query = SMS_GetSingleValue($table, 'COUNT(*)', $condition, null,null, null, true);
        //display_error($query);
        if (!check_empty_result($query))  {
            display_error($msg, true);
            end_page();
            exit;   
        }
    }
}
?>