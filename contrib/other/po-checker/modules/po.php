<?php


function isNonEmptyArray($var) {
	return (isset($var) && is_array($var) && count($var)>0);
}

function isEmptyArray($var) {
	return (isset($var) && is_array($var) && count($var)==0);
}

class po {
	protected $dictionary=array();
	protected $lines=array();
	protected $report=array();
	protected $pot=NULL;

	// line types NOTE MSGID MSGSTR EMPTY PARAMETER
	function load($poFileName, $lang) {
		$this->lang=$lang;
//		echo "<i>Loading $poFileName</i><br />\n";
		$this->dictionary=array();
		$this->lines=array();
		$this->report=array();
		// iterate over lines
		unset($msgid);
		$lastLineType=PO_NONE;
		$linenum=0;
		$extractor="/^((#~ )?(msgid|msgstr|\")|#[\.,:| ]|#$|$)/";
		$extractorLite="/^(msgid|msgstr|\"|#,|$)/";
		$codes=array(
			"msgid" => PO_MSGID,
			"msgstr" => PO_MSGSTR,
			"\"" => PO_DATA,
			"# " => PO_COMMENT, // comment
			"#." => PO_EXTCMNT, // extracted
			"#:" => PO_REFERENCE, // reference
			"#," => PO_FLAGS, // flags
			"" => PO_EMPTY, // empty line
		);

		$block=array();

		// open_file & load lines
		$linesLoaded=file($poFileName);
		$linesLoaded[]=""; // add empty line to enforce block procesing
		foreach ($linesLoaded as $line) {
			$line=trim($line);
			$linenum++;
			$lineType=PO_UNKNOWN;
			$lineRep=false;
#			echo "<br ><br >L:<b>".htmlspecialchars($line)."</b>";
			// #  translator-comments
			// #. extracted-comments
			// #: reference...
			// #, flag...
			// #|  msgid previous-untraslated-string
			// msgid untranslated-string
			// msgstr translated-string
			//  empty line - next translate strings
			if (preg_match_all($extractorLite, $line, $result, PREG_PATTERN_ORDER)) {
				$command=$result[1][0];
				$lineType=$codes[$command];
			} else {
				$lineType=PO_UNKNOWN;
//			 	echo "unknown line in po file<br /><b>$line</b><br />";
			 	continue;
			}
			if (preg_match_all("/^[^\"]*\"(.*)\"[^\"]*$/", $line, $result, PREG_PATTERN_ORDER)) {
				$data=$result[1][0];
			} else {
				$data=substr($line, strlen($command));
			}
#			echo "<br >C:<i>".$command."</i>";
#			echo "<br >D:<i><b>".$data."</b></i>";
#			echo "<br />".$lineType;
			// store lines
//			$this->lines[]=array($lineType, $line);
			switch($lineType) {
			 default:
//			 case PO_MSGID:
//			 case PO_MSGSTR:
				$block[$lineType].=$data;
				if (!isset($blockFirstLine)) {
					$blockFirstLine=$linenum;
				}
				break;

			 case PO_DATA:
				$lineType=$lastLineType;
				$block[$lineType].=$data;
				break;

			 case PO_EMPTY:
				if (count($block) && isset($block[PO_MSGID]) /*&& isset($block[PO_MSGSTR])*/) {
					$msgid=$block[PO_MSGID];
					$msgstr=$block[PO_MSGSTR];
					$flags=array();
					$flagsString=$block[PO_FLAGS];
					$flagsStrings=explode(",", $flagsString);
					foreach ($flagsStrings as &$value) {
						$value=trim($value);
					}
					$flags=array_flip($flagsStrings);
					if (count($flagsStrings)>1 && false) {
						echo "$flagsString :";
						var_dump($flagsStrings);
						echo "<br />";
						var_dump($flags);
						echo "<br />";
					}
/*					while ($flagString=="") {
						$len=strpos($flagString, ",");
						if (!$len) {
							$len=strlen($flagString);
						}
						$flag=;
						$flags[]=$flag
						$flagString=substr($flagString, $len);
					}//*/
//					echo $flagsString;
					$this->dictionary[$msgid]=array("text"=>$msgstr, "line"=>$blockFirstLine, "flag"=>$flags);
					unset($block);
					unset($blockFirstLine);
				}
				break;

			}
			$lastLineType=$lineType;
		}
	}

	function AddPot($pot) {
		if (is_a($pot, "po")) {
			$this->pot=$pot;
		} else if (is_string($pot)) {
			$this->pot=new po;
			$this->pot->Load($pot);
		} else {
			echo "<i>Internal warning</i>: <b>unsupporter pot type</b>";
		}
	}


	function buildPotReport() {
		$search=array("\\n");
		$replace=array("\\n<br />");

		$report=array();
		$report["type"]="pot";
		$report["tot"]=$this->getStringCount();

		// wrong new lines
		$data=array();
		$regexp="/(\\\\r\\\\n|\\\\n|\\\\r)/";
		// make new line style seft test
		foreach ($this->dictionary as $key=>$value) {
			$string=$key;
			$match=preg_match_all($regexp, $string, $matches, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
			if ($match>1) {
				$match=true;
				foreach ($matches[0] as $key2=>$value) {
					$ok=$matches[0][$key2][0]==$matches[0][0][0];
					if (!$ok) {
						$match=false;
					}
					$matches[0][$key2][2]=$ok;
				}

				if (!$match) {
					for ($i=count($matches[0])-1; $i>=0; $i--) {
						$param=$matches[0][$i][0];
						$pos=$matches[0][$i][1];
						$class=$matches[0][$i][2] ? "elmark" : "elerror";
						$string=substr($string, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($string, $pos+strlen($param));
					}

					$line=$this->dictionary[$key]["line"];
					$string=str_replace($search, $replace, $string);
					$data[]=array(count($data)+1, $line, $string);
				}
			}
		}
		$header=array("Index", "Line", "English");
		$table=new Table;
		$table->name="Wrong Newlines";
		$table->header=$header;
		$table->data=$data;
		$report["nls"]=$table;

		// esc aped characters
		$data=array();
		$regexp="/(\\\\.)/";
		$regexp2="/\\\\[\\\\ntr\"]/";
		foreach ($this->dictionary as $key=>$value) {
			$string=$key;
			$match=preg_match_all($regexp, $string, $matches, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
			if ($match>1) {
				$match=true;
				foreach ($matches[0] as $key2=>$value) {
					$match2=preg_match_all($regexp2, $matches[0][$key2][0], $matches2, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
					if ($match2==false) {
						$match=false;
					}
					$matches[0][$key2][2]=$match2!=false;
				}

				if (!$match) {
					for ($i=count($matches[0])-1; $i>=0; $i--) {
						$param=$matches[0][$i][0];
						$pos=$matches[0][$i][1];
						$class=$matches[0][$i][2] ? "elmark" : "elerror";
						$string=substr($string, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($string, $pos+strlen($param));
					}

					$line=$this->dictionary[$key]["line"];
					$string=str_replace($search, $replace, $string);
					$data[]=array(count($data)+1, $line, $string);
				}
			}
		}
		$table=new Table;
		$table->name="Escapes";
		$table->header=array("Index", "Line", "English", "Line", "Native");
		$table->data=$data;
		$report["esc"]=$table;

		$this->report=$report;
		return true;
	}

	function buildReport($potFile=NULL) {
		// clean potFile if unusable 
		$search=array("\\n");
		$replace=array("\\n<br />");
		if ($potFile==NULL) {
			if (isset($this->pot)) {
				$potFile=$this->pot;
			} else {
				return $this->buildPotReport();
			}
		}
		if (is_string($potFile)) {
			$potFileName=$potFile;
			$potFile=new po;
			$potFile->Load($potFileName);
		}
		if (!is_a($potFile, "po")) {
			return false;
		}

		// no report no created && are there data to create report ?
		if (isNonEmptyArray($this->report) || !isNonEmptyArray($this->dictionary)) {
			return true;
		}

		$report["tot"]=$potFile->getStringCount();


		//acc check
		$data=array();
		$regexp="/(&.)/u";
		foreach ($this->dictionary as $orig=>$value) { // check all phrases
			$native=$value["text"];
			if (!isset($native) || $native=="" || $orig=="") { // no translation -> no test
				continue;
			}
			// native and eng did not match in containing of '&'
			if (substr_count($orig, "&")!==substr_count($native, "&")) {
				$engMatch=preg_match_all($regexp, $orig, $matchesOnEng, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				$natMatch=preg_match_all($regexp, $native, $matchesOnNat, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				$lineN=$value["line"];
				$lineE=isset($potFile) ? $potFile->dictionary[$orig]["line"] : ":";
				if (isset($value["flag"]["fuzzy"])) {
					$lineN.="(Fuzzy)";
				}
				for ($i=count($matchesOnEng[0])-1; $i>=0; $i--) {
					$param=$matchesOnEng[0][$i][0];
					$pos=$matchesOnEng[0][$i][1];
					$class=$matchesOnEng[0][$i][2] ? "elmark" : "elerror";
					$orig=substr($orig, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($orig, $pos+strlen($param));
				}
				for ($i=count($matchesOnNat[0])-1; $i>=0; $i--) {
					$param=$matchesOnNat[0][$i][0];
					$pos=$matchesOnNat[0][$i][1];
					$class=$matchesOnNat[0][$i][2] ? "elmark" : "elwarning";
					$native=substr($native, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($native, $pos+strlen($param));
				}
				$orig=str_replace($search, $replace, $orig);
				$native=str_replace($search, $replace, $native);
				$data[]=array(count($data)+1, $lineE, $orig, $lineN, $native);
			}
		}
		$table=new Table;
		$table->name="Misssing/Unexpected Accs";
		$table->header=array("Index", "Line", "English", "Line", "Native");
		$table->data=$data;
		$report["acc"]=$table;

		// wrong new line style
		$data=array();
		$regexp="/(\\\\r\\\\n|\\\\n|\\\\r)/";
		if (isset($potFile)) {
			foreach ($potFile->dictionary as $key=>$value) {
				$orig=$key;
				if (!isset($orig) || $orig=="") {
					continue;
				}
				$native=$this->dictionary[$key]["text"];
				if (!isset($native) || $native=="") {
					continue;
				}
				$engMatch=preg_match_all($regexp, $orig, $matchesOnEng, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				$natMatch=preg_match_all($regexp, $native, $matchesOnNat, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				if ($engMatch || $natMatch) {
//					$match=(count($matchesOnEng[0])==count($matchesOnNat[0]) ? true : false);
					$match=true;
					if ($engMatch) {
						$nls=$matchesOnEng[0][0][0];
					} else {
						$match=false;
						$nls=$matchesOnNat[0][0][0];
					}
					foreach ($matchesOnEng[0] as $key2=>$value2) {
						$ok=$matchesOnEng[0][$key2][0]==$nls;
						if (!$ok) {
							$match=false;
						}
						$matchesOnEng[0][$key2][2]=$ok;
					}
					foreach ($matchesOnNat[0] as $key2=>$value2) {
						$ok=$matchesOnNat[0][$key2][0]==$nls;
						if (!$ok) {
							$match=false;
						}
						$matchesOnNat[0][$key2][2]=$ok;
					}
					if (!$match) {
						for ($i=count($matchesOnEng[0])-1; $i>=0; $i--) {
							$param=$matchesOnEng[0][$i][0];
							$pos=$matchesOnEng[0][$i][1];
							$class=$matchesOnEng[0][$i][2] ? "elmark" : "elerror";
							$orig=substr($orig, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($orig, $pos+strlen($param));
						}
						for ($i=count($matchesOnNat[0])-1; $i>=0; $i--) {
							$param=$matchesOnNat[0][$i][0];
							$pos=$matchesOnNat[0][$i][1];
							$class=$matchesOnNat[0][$i][2] ? "elmark" : "elerror";
							$native=substr($native, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($native, $pos+strlen($param));
						}

						$orig=str_replace($search, $replace, $orig);
						$native=str_replace($search, $replace, $native);
						$lineN=$this->dictionary[$key]["line"];
						$lineE=$potFile->dictionary[$key]["line"];
						if (isset($this->dictionary[$key]["flag"]["fuzzy"])) {
							$lineN.="(Fuzzy)";
						}
						$data[]=array(count($data)+1, $lineE, $orig, $lineN, $native);
					}
				}
			}
		}
		$table=new Table;
		$table->name="Wrong Newlines";
		$table->header=array("Index", "Line", "English", "Line", "Native");
		$table->data=$data;
		$report["nls"]=$table;

		// escaped characters
		$data=array();
		$regexp="/(\\\\.)/";
		$regexp2="/\\\\[\\\\ntr\"]/";
		if (isset($potFile)) {
			foreach ($potFile->dictionary as $key=>$value) {
				$orig=$key;
				if (!isset($orig) || $orig=="") {
					continue;
				}
				$native=$this->dictionary[$key]["text"];
				if (!isset($native) || $native=="") {
					continue;
				}

				$engMatch=preg_match_all($regexp, $orig, $matchesOnEng, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				$natMatch=preg_match_all($regexp, $native, $matchesOnNat, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				if ($engMatch || $natMatch) {
//					$match=(count($matchesOnEng[0])==count($matchesOnNat[0]) ? true : false);
					$match=true;
					// build list of escapes in eng
					unset($list);
					foreach ($matchesOnEng[0] as $key2=>$value2) {
						$list[$matchesOnEng[0][$key2][0]]++;
					}
					// mark unknown chars
					foreach ($matchesOnNat[0] as $key2=>$value2) {
						$element=$matchesOnNat[0][$key2][0];
						$match2=preg_match_all($regexp2, $element, $matches2, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
						if (($match2==false) || (!isset($list[$element]))) {
							$match=false;
							$matchesOnNat[0][$key2][2]=false;
						}
					}
					// if error found add line to data
					if (!$match) {
						for ($i=count($matchesOnEng[0])-1; $i>=0; $i--) {
							$param=$matchesOnEng[0][$i][0];
							$pos=$matchesOnEng[0][$i][1];
							$mark=$matchesOnEng[0][$i][2];
							$class=(!isset($mark) || $mark==true) ? "elmark" : "elerror";
							$orig=substr($orig, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($orig, $pos+strlen($param));
						}

						for ($i=count($matchesOnNat[0])-1; $i>=0; $i--) {
							$param=$matchesOnNat[0][$i][0];
							$pos=$matchesOnNat[0][$i][1];
							$mark=$matchesOnNat[0][$i][2];
							$class=(!isset($mark) || $mark==true) ? "elmark" : "elerror";
							$native=substr($native, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($native, $pos+strlen($param));
						}

						$orig=str_replace($search, $replace, $orig);
						$native=str_replace($search, $replace, $native);
						$lineN=$this->dictionary[$key]["line"];
						$lineE=$potFile->dictionary[$key]["line"];
						if (isset($this->dictionary[$key]["flag"]["fuzzy"])) {
							$lineN.="(Fuzzy)";
						}
						$data[]=array(count($data)+1, $lineE, $orig, $lineN, $native);
					}
				}
			}
		}
		$table=new Table;
		$table->name="Wrong Newlines";
		$table->header=array("Index", "Line", "English", "Line", "Native");
		$table->data=$data;
		$report["esc"]=$table;

		// wrong parameters
		$data=array();
		if (isset($potFile)) {
			// todo: check only "c-format" strings -> speed up
			$regexp="/%(%|[1-9]|[0-9*]*[diouxXsS]|[+\\-]*[0-9*]*?l?d|I64d|[+\\-]?[0-9]*\\.?[0-9]*f|[A-Z]*%)/";
			foreach ($potFile->dictionary as $key=>$value) {
				$orig=$key;
				if (!isset($orig) || $orig=="") {
					continue;
				}
				$native=$this->dictionary[$key]["text"];
				if (!isset($native) || $native=="") {
					continue;
				}
				$engMatch=preg_match_all($regexp, $orig, $matchesOnEng, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				$natMatch=preg_match_all($regexp, $native, $matchesOnNat, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				if ($engMatch || $natMatch) {
					$match=(count($matchesOnEng[0])==count($matchesOnNat[0]) ? true : false);
					foreach ($matchesOnEng[0] as $key2=>$value2) {
						$ok=($matchesOnEng[0][$key2][0]==$matchesOnNat[0][$key2][0]);
						if (!$ok) {
							$match=false;
						}
						$matchesOnEng[0][$key2][2]=$ok;
					}
					foreach ($matchesOnNat[0] as $key2=>$value2) {
						$ok=($matchesOnEng[0][$key2][0]==$matchesOnNat[0][$key2][0]);
						if (!$ok) {
							$match=false;
						}
						$matchesOnNat[0][$key2][2]=$ok;
					}
					if (!$match) {
						for ($i=count($matchesOnEng[0])-1; $i>=0; $i--) {
							$param=$matchesOnEng[0][$i][0];
							$pos=$matchesOnEng[0][$i][1];
							$class=$matchesOnEng[0][$i][2] ? "elmark" : "elerror";
							$orig=substr($orig, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($orig, $pos+strlen($param));
						}
						for ($i=count($matchesOnNat[0])-1; $i>=0; $i--) {
							$param=$matchesOnNat[0][$i][0];
							$pos=$matchesOnNat[0][$i][1];
							$class=$matchesOnNat[0][$i][2] ? "elmark" : "elerror";
							$native=substr($native, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($native, $pos+strlen($param));
						}
						$orig=str_replace($search, $replace, $orig);
						$native=str_replace($search, $replace, $native);
						$lineN=$this->dictionary[$key]["line"];
						$lineE=$potFile->dictionary[$key]["line"];
						if (isset($this->dictionary[$key]["flag"]["fuzzy"])) {
							$lineN.="(Fuzzy)";
						}
						$data[]=array(count($data)+1, $lineE, $orig, $lineN, $native);
					}
				}
			}
		}
		$table=new Table;
		$table->name="Wrong Params";
		$table->header=array("Index", "Line", "English", "Line", "Native");
		$table->data=$data;
		$report["par"]=$table;


		// wrong parameters
		$data=array();
		if (isset($potFile)) {
			// todo: check only "c-format" strings -> speed up
			$regexp="/(%(%|[1-9]|[0-9*]*[diouxXsS]|[+\\-]*[0-9*]*?l?d|I64d|[+\\-]?[0-9]*\\.?[0-9]*f|[A-Z]*%))/";
			$regexp="/(%(%|[1-9]|[0-9*]*[diouxXsS]|[+\\-]*[0-9*]*?l?d|I64d|[+\\-]?[0-9]*\\.?[0-9]*f|[A-Z]*%))|(\\\\[\\\\nr\"])/";
			foreach ($potFile->dictionary as $key=>$value) {
				$orig=$key;
				if (!isset($orig) || $orig=="") {
					continue;
				}
				$native=$this->dictionary[$key]["text"];
				if (!isset($native) || $native=="") {
					continue;
				}
				$engMatch=preg_match_all($regexp, $orig, $matchesOnEng, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				$natMatch=preg_match_all($regexp, $native, $matchesOnNat, PREG_PATTERN_ORDER | PREG_OFFSET_CAPTURE);
				if ($engMatch || $natMatch) {
					$haveNewLine=false;
					$haveParam=false;
					$match=true;
					foreach ($matchesOnEng[3] as $key2=>$value2) {
						if ($value2[0]!="") {
//							echo "<pre>".var_dump($value2)."</pre>";
							$haveNewLine=true;
						}
					}
					foreach ($matchesOnEng[1] as $key2=>$value2) {
						if ($value2[0]!="") {
							$haveParam=true;
						}
					}
					if ($haveParam==false || $haveNewLine==false) { // skip translations whot dont have both param and new line
						continue;
					}
					foreach ($matchesOnEng[0] as $key2=>$value2) {
						$ok=($matchesOnEng[0][$key2][0]==$matchesOnNat[0][$key2][0]);
						if (!$ok) {
							$match=false;
						}
						$matchesOnEng[0][$key2][2]=$ok;
					}
					foreach ($matchesOnNat[0] as $key2=>$value2) {
						$ok=($matchesOnEng[0][$key2][0]==$matchesOnNat[0][$key2][0]);
						if (!$ok) {
							$match=false;
						}
						$matchesOnNat[0][$key2][2]=$ok;
					}
					if (!$match) {
						for ($i=count($matchesOnEng[0])-1; $i>=0; $i--) {
							$param=$matchesOnEng[0][$i][0];
							$pos=$matchesOnEng[0][$i][1];
							$class=$matchesOnEng[0][$i][2] ? "elmark" : "elerror";
							$orig=substr($orig, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($orig, $pos+strlen($param));
						}
						for ($i=count($matchesOnNat[0])-1; $i>=0; $i--) {
							$param=$matchesOnNat[0][$i][0];
							$pos=$matchesOnNat[0][$i][1];
							$class=$matchesOnNat[0][$i][2] ? "elmark" : "elerror";
							$native=substr($native, 0, $pos)."<font class=\"$class\">".$param."</font>".substr($native, $pos+strlen($param));
						}
						$orig=str_replace($search, $replace, $orig);
						$native=str_replace($search, $replace, $native);
						$lineN=$this->dictionary[$key]["line"];
						$lineE=$potFile->dictionary[$key]["line"];
						if (isset($this->dictionary[$key]["flag"]["fuzzy"])) {
							$lineN.="(Fuzzy)";
						}
						$data[]=array(count($data)+1, $lineE, $orig, $lineN, $native);
					}
				}
			}
		}
		$table=new Table;
		$table->name="Wrong Params Line";
		$table->header=array("Index", "Line", "English", "Line", "Native");
		$table->data=$data;
//		$report["p2l"]=$table;


		// non traslated
		$data=array();
		if (isset($potFile)) {
			foreach ($potFile->dictionary as $key=>$value) {
				$orig=$key;
				$native=$this->dictionary[$key]["text"];
				if (!isset($native) || $native=="") {
					$lineN=$this->dictionary[$key]["line"];
					$lineE=$potFile->dictionary[$key]["line"];
					$orig=str_replace($search, $replace, $orig);
					$data[]=array(count($data)+1, $lineE, $orig, $lineN);
				}
			}
		}
		$table=new Table;
		$table->name="Nottranslated";
		$table->header=array("Index", "Line", "English", "Line");
		$table->data=$data;
		$report["unt"]=$table;


		// fuzzy
		$data=array();
		foreach ($this->dictionary as $key=>$value) {
			$orig=$key;
			$flags=$value["flag"];
			if (!isset($flags) || count($flags)<1) {
				continue;
			}
			if (isset($flags) && isset($flags["fuzzy"])) {
				$lineN=$this->dictionary[$key]["line"];
				$native=$value["text"];
				$orig=str_replace($search, $replace, $orig);
				$native=str_replace($search, $replace, $native);
				$data[]=array(count($data)+1, "-", $orig, $lineN, $native);
			}
		}
		$table=new Table;
		$table->name="Fuzzy";
		$table->header=array("Index", "Line", "English", "Line", "Native");
		$table->data=$data;
		$report["fuz"]=$table;

		$this->report=$report;
		return true;
	}

	function GetErrorTypes() {
		return array_keys($this->report);
	}

	function GetErrorCount($name) {
		$count=$this->report[$name];
		if (!isset($count)) {
			return false;
		}
		if (is_a($count, "Table")) {
			$count=count($count->data);
		}
		if (is_array($count)) {
			$count=count($count);
		}
		return $count;
	}

	function save($poFileName="") {
		if ($poFileName=="") { // export to browser
		} else { // export to file
		}
	}

	function translate($str) {
		return $this->dictionary[$str]["text"];
	}

	// update from .pot file
	function update($poFileName){
		
	}

	function addTranslate($origText, $nativeText){
	}

	function getTranslate($origText){
	}

	function getAllOrigs(){
	}

	function getProgress(){
		buildReport();
		return $this->report();
		return array(
				"translate" => $this->report["unt"]);
	}

	function getStringCount(){
		return count($this->dictionary);
	}

	function printReport($potFile=NULL) {
		$this->buildReport($potFile);

		if (isset($this->report["param"])) {
			$this->report["param"]->output();
		} else {
		}

		if ($this->report["type"]=="pot") {
			echo "<a name=\"nls$lang\"></a><h3>Differences in new line style within one string</h3>\n";
//			echo "<p>
//				</p>\n";
			$table=$this->report["nls"];
			if (isset($table)) {
				if (count($table->data)) { // table exists and has data
					$table->output();
					echo "<sup>Note: Line numbers are as in commited file, it may differ when you update to use newer .pot, also edited multiline messages may shift line position.</sup>\n";
					echo "<p>Total:".count($table->data)."</p>\n";
				} else {
					echo "<p><b>PASS</b></p>\n";
				}
			} else {
				echo "<p><i>Internal error</i></p>";
			}

			echo "<a name=\"esc$lang\"></a><h3>Escaped chars</h3>\n";
//			echo "<p>
//				</p>\n";
			$table=$this->report["esc"];
			if (isset($table)) {
				if (count($table->data)) { // table exists and has data
					$table->output();
					echo "<sup>Note: Line numbers are as in commited file, it may differ when you update to use newer .pot, also edited multiline messages may shift line position.</sup>\n";
					echo "<p>Total:".count($table->data)."</p>\n";
				} else {
					echo "<p><b>PASS</b></p>\n";
				}
			} else {
				echo "<p><i>Internal error</i></p>";
			}
			return;
		}

		// clean potFile if unusable 
		// todo: check class
		if ($potFile==NULL) {
			unset($potFile);
		}

		$lang=$this->lang;

		echo "<a name=\"par$lang\"></a><h3>Parameter strings test</h3>\n";
		echo "<p>This test is necessary to pass or you can expect crashing of TSVN - specially true when you use 
				longer type as is in English like %s instead of %d, %ld instead of %d, etc. Also having more parameters lead to crash TSVN.
				When TSVN not crash, the information is still wrong. For example number instead of text.</p>\n";
		$table=$this->report["par"];
		if (isset($table)) {
			if (count($table->data)) { // table exists and has data
				$table->output();
				echo "<sup>Note: Line numbers are as in committed file, it may differ when you update to use newer .pot, also edited multi line messages may shift line position.</sup>\n";
				echo "<p>Total:".count($table->data)."</p>\n";
			} else {
				echo "<p><b>PASS</b></p>\n";
			}
		} else {
			echo "<p><i>Internal error</i></p>";
		}

/*		echo "<a name=\"par$lang\"></a><h3>Parameters to lines test</h3>\n";
		echo "<p>If you pass previous test this one is only for ... </p>\n";
		$table=$this->report["p2l"];
		if (isset($table)) {
			if (count($table->data)) { // table exists and has data
				$table->output();
				echo "<sup>Note: Line numbers are as in committed file, it may differ when you update to use newer .pot, also edited multi line messages may shift line position.</sup>\n";
				echo "<p>Total:".count($table->data)."</p>\n";
			} else {
				echo "<p><b>PASS</b></p>\n";
			}
		} else {
			echo "<p><i>Internal error</i></p>";
		}//*/

		echo "<a name=\"acc$lang\"></a><h3>Missing/Unexpected Key Accelerator test</h3>\n";
		echo "<p>This is accessibility test. Passing this test is recommended. 
			If you are interesting in more details about accelerator key overlapping I may enable such test for your language, but in fact there not too much to do against it.
			According <a href=\"http://tortoisesvn.net/user/2\">Luebbe</a> and <a href=\"http://tortoisesvn.net/user/3\">Stefan</a> 
			there is no problem about it you just have to use accelerator more time to select element and then use enter or space for activate button.
			</p>\n";
		$table=$this->report["acc"];
		if (isset($table)) {
			if (count($table->data)) { // table exists and has data
				$table->output();
				echo "<sup>Note: Line numbers are as in committed file, it may differ when you update to use newer .pot, also edited multi line messages may shift line position.</sup>\n";
				echo "<p>Total:".count($table->data)."</p>\n";
			} else {
				echo "<p><b>PASS</b></p>\n";
			}
		} else {
			echo "<p><i>Internal error</i></p>";
		}

		echo "<a name=\"nls$lang\"></a><h3>Differences in new line style</h3>\n";
		echo "<p>This tests if new line style from pot match localized new line style. <b>This test may have false positives when English text has new line style inconsistence.</b> 
				Using more lines for Native when English use just one is also alowed for most of translation even here reported.
			</p>\n";
		$table=$this->report["nls"];
		if (isset($table)) {
			if (count($table->data)) { // table exists and has data
				$table->output();
				echo "<sup>Note: Line numbers are as in committed file, it may differ when you update to use newer .pot, also edited multi line messages may shift line position.</sup>\n";
				echo "<p>Total:".count($table->data)."</p>\n";
			} else {
				echo "<p><b>PASS</b></p>\n";
			}
		} else {
			echo "<p><i>Internal error</i></p>";
		}

		echo "<a name=\"unt$lang\"></a><h3>Not translated strings test</h3>\n";
		echo "<p>This tests if all strings has been translated. Translated mean that there in native string set. If English and native are same it is not marked as not translated, this just can happen.</p>\n";
		$table=$this->report["unt"];
		if (isset($table)) {
			if (count($table->data)) { // table exists and has data
				$table->output();
				echo "<sup>Note: Line numbers are as in committed file, it may differ when you update to use newer .pot, also edited multi line messages may shift line position.</sup>\n";
				echo "<p>Total:".count($table->data)."</p>\n";
			} else {
				echo "<p><b>PASS</b></p>\n";
			}
		} else {
			echo "<p><i>Internal error</i></p>";
		}

		echo "<a name=\"fuz$lang\" /><h3>Fuzzy mark test</h3>\n";
		echo "<p>String marked as fuzzy is not in final shape.</p>\n";
		$table=$this->report["fuz"];
		if (isset($table)) {
			if (count($table->data)) { // table exists and has data
				$table->output();
				echo "<sup>Note: Line numbers are as in committed file, it may differ when you update to use newer .pot, also edited multi line messages may shift line position.</sup>\n";
				echo "<p>Total:".count($table->data)."</p>\n";
			} else {
				echo "<p><b>PASS</b></p>\n";
			}
		} else {
			echo "<p><i>Internal error or not implemented</i></p>";
		}

		echo "<a name=\"esc$lang\" /><h3>Escaped chars test</h3>\n";
		echo "<p>This test check if all escaped chars are known and its using match with English. Even you get red here it may be legal for your language.<br/>This test is in developing now.</p>\n";
		$table=$this->report["esc"];
		if (isset($table)) {
			if (count($table->data)) { // table exists and has data
				$table->output();
				echo "<sup>Note: Line numbers are as in committed file, it may differ when you update to use newer .pot, also edited multi line messages may shift line position.</sup>\n";
				echo "<p>Total:".count($table->data)."</p>\n";
			} else {
				echo "<p><b>PASS</b></p>\n";
			}
		} else {
			echo "<p><i>Internal error or not implemented</i></p>";
		}

	}
}



//php?>

