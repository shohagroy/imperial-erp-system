<?php

class customCart extends Cart {

	var $total_discount;

	function write($policy=0) {
		
		global $SysPrefs, $Refs;
		
		begin_transaction(); // prevents partial database changes in case of direct delivery/invoice
		if ($this->reference != 'auto' && $this->trans_no == 0 && !is_new_reference($this->reference, $this->trans_type))
		{
			if (!empty($SysPrefs->prefs['ref_no_auto_increase']))
				$this->reference = $Refs->get_next($this->trans_type, null, array('date' => Today()));
			else	
			{
				commit_transaction();
				return -1;
			}
		}
		if (count($this->src_docs) == 0 && ($this->trans_type == ST_SALESINVOICE || $this->trans_type == ST_CUSTDELIVERY) && !$this->is_prepaid()) {
			// this is direct document - first add parent
			$ref = $this->reference;
			$date = $this->document_date;
			$due_date = $this->due_date;
			$dimension_id = $this->dimension_id;
			$dimension2_id = $this->dimension2_id;
			$this->trans_type = get_parent_type($this->trans_type);

			$this->reference = 'auto'; 
			$trans_no = $this->write(1); 

			// re-read parent document converting it to child
			$this->read($this->trans_type, $trans_no, true); 
			$this->document_date = $date;
			$this->reference = $ref;
			$this->due_date = $due_date;
			$this->dimension_id = $dimension_id;
			$this->dimension2_id = $dimension2_id;
		}
		$this->reference = @html_entity_decode($this->reference, ENT_QUOTES);
		$this->Comments = @html_entity_decode($this->Comments, ENT_QUOTES);
		foreach($this->line_items as $lineno => $line) {
			$this->line_items[$lineno]->stock_id = @html_entity_decode($line->stock_id, ENT_QUOTES);
			$this->line_items[$lineno]->item_description = @html_entity_decode($line->item_description, ENT_QUOTES);
		}
		switch($this->trans_type) {
			case ST_SALESINVOICE:
				$ret = write_partial_sales_invoice($this);
				break;
			case ST_CUSTCREDIT:
				$ret = write_credit_note($this, $policy);
				break;
			case ST_CUSTDELIVERY:
				$ret = write_sales_delivery($this, $policy);
				break;
			case ST_SALESORDER:
			case ST_SALESQUOTE:
				if ($this->trans_no==0)	// new document
					$ret = add_sales_order($this);
				else
					$ret = update_sales_order($this);
		}

		commit_transaction();

		return $ret;
	}
}