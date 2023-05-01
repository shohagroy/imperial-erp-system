<?php

global $path_to_root;
include($path_to_root.'/reporting/includes/doctext.inc');

$this->row = $this->pageHeight - $this->topMargin;
$upper = $this->row - 2 * $this->lineHeight;
$lower = $this->bottomMargin + 2 * $this->lineHeight;
$iline1 = $upper - 7.5 * $this->lineHeight;
$iline2 = $iline1 - 8 * $this->lineHeight;
$iline3 = $iline2 - 1.5 * $this->lineHeight;
$iline4 = $iline3 - 1.5 * $this->lineHeight;
$iline5 = $iline4 - 3 * $this->lineHeight;
$iline6 = $iline5 - 1.5 * $this->lineHeight;
$iline7 = $lower + 3 * $this->lineHeight;
$right = $this->pageWidth - $this->rightMargin;
$width = ($right - $this->leftMargin) / 5;
$page_width = $this->pageWidth - $this->leftMargin - $this->rightMargin;
$page_height = $this->pageHeight - $this->topMargin - $this->bottomMargin;
$icol = $this->pageWidth / 2;
$ccol = $this->cols[0] + 4;
$c2col = $ccol + 60;
$ccol2 = $icol / 2;
$mcol = $icol + 8;
$mcol2 = $this->pageWidth - $ccol2;
$cols = count($this->cols);

$this->SetDrawColor(128, 128, 128);

$this->Line($iline3);
$this->Line($iline4);
$this->Line($iline7 + 9.5*$this->lineHeight);
$this->LineTo($this->leftMargin, $iline3 , $this->leftMargin, $iline7 + 9.5*$this->lineHeight);
if ($this->l['a_meta_dir'] == 'rtl') {// avoid line overwrite in rtl language
	$this->LineTo($this->cols[$cols - 2], $iline3 , $this->cols[$cols - 2], $iline7 + 9.5*$this->lineHeight);
}
else {
	$this->LineTo($this->cols[$cols - 6]-2, $iline3 , $this->cols[$cols - 6]-2, $this->formData['line_bottom']);
	$this->LineTo($this->cols[$cols - 5], $iline3 , $this->cols[$cols - 5], $this->formData['line_bottom']);
	$this->LineTo($this->cols[$cols - 4], $iline3 , $this->cols[$cols - 4], $this->formData['line_bottom']);
	$this->LineTo($this->cols[$cols - 3], $iline3 , $this->cols[$cols - 3], $this->formData['line_bottom']);
	$this->LineTo($this->cols[$cols - 2]+2, $iline3 , $this->cols[$cols - 2]+2, $iline7 + 9.5*$this->lineHeight);
}	

$this->LineTo($right, $iline3 ,$right, $iline7 + 9.5*$this->lineHeight);

//Terms & Condition
$this->Font('bold');
$this->TextWrap($this->cols[0], $iline7 + 8*$this->lineHeight, $this->pageWidth, _('Terms & Conditions'), 'left');
$this->Font();

// Company Logo && Addresses
$logo = company_path().'/images/'.$this->company['coy_logo'];
$this->Font('bold');
$this->fontSize += 10;
if ($this->company['coy_logo'] != '' && file_exists($logo))
	$this->Image($logo, 0, $this->topMargin, '', 45, '', '', '', false, '300', 'C');
else {
	$this->SetTextColor(100, 100, 100);
	$this->TextWrap(0, $this->row - $this->topMargin + $this->lineHeight, $this->pageWidth, $this->company['coy_name'], 'center');
}
$this->fontSize -= 8;
$this->NewLine(5);
$this->TextWrap(0, $this->row, $this->pageWidth, $this->company['postal_address'], 'center');
$phone = empty($this->company['phone']) ? '' : _('Phone: ').$this->company['phone'].', ';
$email = empty($this->company['email']) ? '' : _('E-mail: ').$this->company['email'].', ';
$fax = empty($this->company['fax']) ? '' : _('Fax: ').$this->company['fax'].', ';
$contacts = $phone.$fax.$email;
$this->NewLine(2);
$this->TextWrap(0, $this->row, $this->pageWidth, $contacts, 'center');
$this->Font();
$this->fontSize -= 2;

// Document title
$this->SetTextColor(100, 100, 100);
$this->fontSize += 10;
$this->Font('bold');
$this->NewLine(3);
$this->TextWrap(0, $this->row, $this->pageWidth, $this->title, 'center');
$this->Font();
$this->fontSize -= 10;
$this->NewLine();
$this->SetTextColor(50, 50, 50);

// info left
// $trans = get_partial_payment_trans($this->formData['trans_no'], $this->formData['doctype']);
$temp = $this->row = $this->row - 2*$this->lineHeight;
$this->Text($ccol, _('Customer reference:'));
$this->Text($ccol + 100, $this->formData["customer_ref"]);
$this->NewLine();
$this->Text($ccol, _('Customer: '));
$this->Text($ccol + 100, $this->formData['DebtorName']);
$this->NewLine();
$this->Text($ccol, _('Address:'));
$addrs = isset($this->formData['address']) ? $this->formData['address'] : '';
$this->TextWrapLines($ccol + 100, $icol - $ccol - 50, $addrs);
$this->Text($ccol, _('Contact Phone: '));
$cust_phone = empty($this->formData['contact_phone']) ? '' : $this->formData['contact_phone']; //contact_phone set from report file
if(empty($cust_phone))
	$cust_phone = empty($this->contactData[0]['phone']) ? $this->contactData[0]['phone2'] : $this->contactData[0]['phone'];
$this->Text($ccol + 100, $cust_phone);

// info right
$this->row = $temp;
$this->Text($mcol + 60, _('Reference (No):'));
$this->Text($mcol + 130, $this->formData['reference'].' ('.$this->formData['trans_no'].')');
$this->NewLine();
$this->Text($mcol + 60, _('Created Time:'));
$this->Text($mcol + 130, sql2date($this->formData['tran_date']).' '.$this->formData['time_stamp']);
$this->NewLine();
$row = get_payment_terms($this->formData['payment_terms']);
$this->Text($mcol + 60, _('Payment type:'));
$this->Text($mcol + 130, $row['terms']);
$this->NewLine();
$this->Text($mcol + 60, _('From Branch:'));
$custom_br = isset($trans['branch_id']) ? get_custom_branches($trans['branch_id']) : '';
$this->Text($mcol + 130, isset($custom_br['branch_name']) ? $custom_br['branch_name'] : '');
$this->NewLine();
$loc_name = isset($this->formData['location_name']) ? $this->formData['location_name'] : '';
$this->Text($mcol + 60, _('From Location:'));
$this->Text($mcol + 130, $loc_name);

// Line headers
$this->headers = array(_('SL#'), _('Item Description'), _('Unit'), _('Quantity'), _('Rate'), _('Amount'));
$this->row = $iline3 - $this->lineHeight - 1;
$this->Font('bold');
$count = count($this->headers);
$this->cols[$count] = $right - 3;
for ($i = 0; $i < $count; $i++)
	$this->TextCol($i, $i + 1, $this->headers[$i], -2);
$this->Font();

// Signature fields
$this->row = $lower - $this->lineHeight;
$this->TextWrap($ccol, $this->row + 2, $ccol + 50, $this->formData['user_id'], 'C');
$this->TextWrap($ccol, $this->row, $mcol, str_pad('', 20, '_'), 'L'); // Prepared by
$this->TextWrap($mcol-50, $this->row, $mcol2 + 50, str_pad('', 20, '_'), 'C'); // Authorized by
$this->NewLine();
$this->Font('bold');
$this->TextWrap($ccol, $this->row, $ccol + 50, _('Prepared By'), 'C');
$this->TextWrap($mcol-50, $this->row, $mcol2 + 50, _('Authorized Signature'), 'C');
$this->Font();

$this->row = $iline4 - $this->lineHeight; //Invoice line item starts