package tw.com.zhenhai.model

object MachineInfo {
  val productMapping = Map(
    "A65"    -> "18",       "A41"    -> "12.5",    "A43"    -> "12.5",     
    "A42"    -> "12.5",     "A51"    -> "12.5",    "A40"    -> "12.5",
    "A47"    -> "12.5",     "A67"    -> "18",      "A63"    -> "12.5",     
    "A66"    -> "12.5",     "A38"    -> "8",       "A48"    -> "18",      
    "A50"    -> "12.5x31",  "A49"    -> "18",      "A62"    -> "8",
    "A58"    -> "5",        "A52"    -> "12.5",    "A44"    -> "18",       
    "A57"    -> "10x16",    "A60"    -> "18",      "A64"    -> "18",
    "A54"    -> "18",       "A53"    -> "18",      "A61"    -> "18",       
    "A55"    -> "18",       "A59"    -> "18",      "A56"    -> "18",
    "A39"    -> "5",        "A30"    -> "8",       "A23"    -> "8",
    "A22"    -> "10",       "A16"    -> "8",       "A15"    -> "8",        
    "A08"    -> "6",        "A07"    -> "5",       "A35"    -> "5",        
    "A35"    -> "5",        "A33"    -> "10",      "A31"    -> "5",        
    "A29"    -> "6",        "A28"    -> "10",      "A21"    -> "6",       
    "A20"    -> "8",        "A14"    -> "6",       "A13"    -> "6",
    "A06"    -> "10",       "A05"    -> "8",       "A36"    -> "5",
    "A34"    -> "5",        "A32"    -> "10",      "A27"    -> "12.5",     
    "A26"    -> "12.5",     "A19"    -> "10",      "A46"    -> "5",        
    "A12"    -> "6",        "A11"    -> "10",      "A04"    -> "5",       
    "A03"    -> "6",        "A25"    -> "10",      "A24"    -> "10",
    "A18"    -> "12.5",     "A17"    -> "8",       "A10"    -> "10",       
    "A09"    -> "10",       "A02"    -> "12.5",    "A01"    -> "8",        
    "G111"   -> "18x50",    "E116"   -> "12.5x20", "E117"   -> "12.5x31",  
    "E118"   -> "18",       "E119"   -> "18",      "E120"   -> "18",      
    "E49-S7" -> "18",       "E49-S6" -> "18",      "E49-S5" -> "18",
    "E49-S4" -> "18",       "E108"   -> "18",      "E49-S3" -> "18",       
    "E49-K1" -> "18",       "G111"   -> "18x50",   "E115"   -> "12.5x25",  
    "E114"   -> "12.5x25",  "E123"   -> "18",      "E122"   -> "18",       
    "E121"   -> "18",       "E49-S8" -> "18",      "E49-S1" -> "18",      
    "E102"   -> "18",       "E101"   -> "18",      "E49-S2" -> "18",
    "E49-K2" -> "18",       "G47"    -> "16",      "G94"    -> "16x25",    
    "G109"   -> "16",       "G77"    -> "6x11",    "E77"    -> "6x11",     
    "G80"    -> "6x11",     "E80"    -> "6x11",    "G75"    -> "6x11",     
    "E75"    -> "6x11",     "G81"    -> "6x11",    "E81"    -> "6x11",    
    "G24"    -> "5x11",     "E24"    -> "5x11",    "G30"    -> "5x11",
    "E30"    -> "5x11",     "G93"    -> "16",      "G108"   -> "18",  
    "G107"   -> "12.5x31",  "E71"    -> "6x11",    "G71"    -> "6x11",     
    "E13"    -> "6x11",     "G13"    -> "6x11",    "E34"    -> "5x11",     
    "G34"    -> "5x11",     "E73"    -> "5x11",    "G73"    -> "5x11",
    "E79"    -> "6x11",     "G79"    -> "6x11",    "E21"    -> "5x11",
    "G21"    -> "5x11",     "G106"   -> "18",      "G49"    -> "18",       
    "G60"    -> "5x11",     "E60"    -> "5x11",    "G61"    -> "5x11",     
    "E61"    -> "5x11",     "E38"    -> "5x11",    "G38"    -> "5x11",     
    "G05"    -> "5x11",     "E05"    -> "5x11",    "G06"    -> "5x11",    
    "E06"    -> "5x11",     "E42"    -> "5x11",    "G42"    -> "5x11",
    "G103"   -> "18",       "E76"    -> "5x11",    "G76"    -> "5x11",     
    "E56"    -> "5x11",     "G56"    -> "5x11",    "E66"    -> "5x11",     
    "G66"    -> "5x11",     "E43"    -> "5x11",    "G43"    -> "5x11",     
    "E70"    -> "5x11",     "G70"    -> "5x11",    "E01"    -> "5x11",    
    "G01"    -> "5x11",     "E39"    -> "10x20",   "G64"    -> "10x12",
    "E64"    -> "10x12",    "G84"    -> "10x12",   "E84"    -> "10x12",    
    "G09"    -> "10x16",    "E09"    -> "10x16",   "G50"    -> "10x16",    
    "E50"    -> "10x16",    "G91"    -> "10x20",   "E91"    -> "10x20",    
    "G92"    -> "10x20",    "E92"    -> "10x20",   "G67"    -> "8x20",    
    "E67"    -> "8x20",     "G100"   -> "10x20",   "E100"   -> "10x20",
    "G62"    -> "10x25",    "E62"    -> "10x25",   "G104"   -> "10x25",    
    "E104"   -> "10x25",    "G20"    -> "10x25",   "E20"    -> "10x25",    
    "G72"    -> "10x25",    "E87"    -> "10x25",   "G95"    -> "10x25",    
    "E11"    -> "10x25",    "G45"    -> "12.5x25", "E112"   -> "10x20",   
    "G46"    -> "12.5x31",  "E88"    -> "16x25",   "G105"   -> "10x31",
    "E45"    -> "10x31",    "G39"    -> "10x20",   "E16"    -> "10x12",    
    "G16"    -> "10x12",    "E31"    -> "10x16",   "G31"    -> "10x16",    
    "E15"    -> "10x16",    "G15"    -> "10x16",   "E63"    -> "10x16",    
    "G63"    -> "10x16",    "E55"    -> "10x16",   "G55"    -> "10x16",   
    "E32"    -> "8x20",     "G32"    -> "8x20",    "E99"    -> "10x20",
    "G99"    -> "10x20",    "E07"    -> "10x20",   "G07"    -> "10x20",    
    "E41"    -> "10x20",    "G41"    -> "10x20",   "E53"    -> "10x20",    
    "G53"    -> "10x20",    "E83"    -> "8x20",    "G83"    -> "8x20",     
    "E19"    -> "8x20",     "G19"    -> "8x20",    "E18"    -> "12.5x20", 
    "G18"    -> "12.5x20",  "E106"   -> "12.5x15", "G97"    -> "12.5x15",
    "E111"   -> "12.5x20",  "G96"    -> "12.5x20", "E113"   -> "12.5x20",  
    "E98"    -> "12.5x20",  "E110"   -> "10x16",   "G33"    -> "8x15",     
    "E33"    -> "8x15",     "G08"    -> "8x15",    "E08"    -> "8x15",     
    "G68"    -> "8x11",     "E68"    -> "8x11",    "G29"    -> "8x11",    
    "E29"    -> "8x11",     "G51"    -> "8x11",    "E51"    -> "8x11",
    "G27"    -> "8x11",     "E27"    -> "8x11",    "G85"    -> "10x16",    
    "E85"    -> "10x16",    "G04"    -> "6x11",    "E04"    -> "6x11",     
    "G57"    -> "6x11",     "E57"    -> "6x11",    "G40"    -> "6x11",     
    "E40"    -> "6x11",     "G22"    -> "6x11",    "E22"    -> "6x11",    
    "G25"    -> "10x20",    "E25"    -> "10x20",   "G78"    -> "10x16",
    "E78"    -> "10x16",    "G82"    -> "8x11",    "E82"    -> "8x11",     
    "G69"    -> "4x7",      "E69"    -> "4x7",     "G26"    -> "6.3x7",    
    "E26"    -> "6.3x7",    "G110"   -> "10x16",   "E36"    -> "8x15",     
    "G36"    -> "8x15",     "E35"    -> "8x15",    "G35"    -> "8x15",    
    "E23"    -> "8x11",     "G23"    -> "8x11",    "E58"    -> "8x11",
    "G58"    -> "8x11",     "E65"    -> "8x11",    "G65"    -> "8x11",     
    "E59"    -> "8x11",     "G59"    -> "8x11",    "E86"    -> "8x11",     
    "G86"    -> "8x11",     "E37"    -> "6x11",    "G37"    -> "6x11",     
    "E17"    -> "6x11",     "G17"    -> "6x11",    "E03"    -> "10x16",   
    "G03"    -> "10x16",    "E12"    -> "6x11",    "G12"    -> "6x11",
    "E44"    -> "8x15",     "G44"    -> "8x15",    "E74"    -> "8x15",     
    "G74"    -> "8x15",     "E10"    -> "8x15",    "G10"    -> "8x15",
    "E54"    -> "10x16",    "G54"    -> "10x16",   "E28"    -> "8x15",     
    "G28"    -> "8x15",     
    
    // Duplicates of E49-*
    "S7"     -> "18",       "S6"     -> "18",      "S5"     -> "18",
    "S4"     -> "18",       "S3"     -> "18",      "K1"     -> "18",         
    "S8"     -> "18",       "S1"     -> "18",      "S2"     -> "18",
    "K2"     -> "18",   

    // Dummy Machine
    "A001"   -> "8x15", "A002" -> "10x16", "A003" -> "12.5x20", "A004" -> "6x11", "A005" -> "4x7",
    "A006"   -> "8x15", "A007" -> "10x16", "A008" -> "12.5x20", "A009" -> "6x11", "A010" -> "4x7",
    "B001"   -> "8x15", "B002" -> "10x16", "B003" -> "12.5x20", "B004" -> "6x11", "B005" -> "4x7",
    "B006"   -> "8x15", "B007" -> "10x16", "B008" -> "12.5x20", "B009" -> "6x11", "B010" -> "4x7",
    "C001"   -> "8x15", "C002" -> "10x16", "C003" -> "12.5x20", "C004" -> "6x11", "C005" -> "4x7",
    "C006"   -> "8x15", "C007" -> "10x16", "C008" -> "12.5x20", "C009" -> "6x11", "C010" -> "4x7",
    "D001"   -> "8x15", "D002" -> "10x16", "D003" -> "12.5x20", "D004" -> "6x11", "D005" -> "4x7",
    "D006"   -> "8x15", "D007" -> "10x16", "D008" -> "12.5x20", "D009" -> "6x11", "D010" -> "4x7",
    "E001"   -> "8x15", "E002" -> "10x16", "E003" -> "12.5x20", "E004" -> "6x11", "E005" -> "4x7",
    "E006"   -> "8x15", "E007" -> "10x16", "E008" -> "12.5x20", "E009" -> "6x11", "E010" -> "4x7"
  )

  val machineType = Map(
    1 -> "加締卷取",
    2 -> "組立",
    3 -> "老化",
    4 -> "加工",
    5 -> "切腳",
    6 -> "包裝"
  )

  val machineModel = Map(
    "A01"  -> ("CS-204K", 2),    "A02"  -> ("CS-206", 2),     "A03"  -> ("CS-205", 2), 
    "A04"  -> ("CS-205", 2),     "A05"  -> ("CS-205", 2),     "A06"  -> ("CS-210", 2), 
    "A07"  -> ("ATS-630J", 3),   "A08"  -> ("CS-210", 3),     "A09"  -> ("CS-206K", 3), 
    "A10"  -> ("CS-204K", 3),    "A11"  -> ("CS-205", 3),     "A12"  -> ("CS-210", 3), 
    "A13"  -> ("CS-210", 3),     "A14"  -> ("CS-210", 3),     "A15"  -> ("ATS-720", 3), 
    "A16"  -> ("ATS-720", 3),    "A17"  -> ("CS-205", 3),     "A18"  -> ("CS-206", 3), 
    "A19"  -> ("CS-210", 3),     "A20"  -> ("CS-210", 3),     "A21"  -> ("CS-210", 3), 
    "A22"  -> ("CS-210", 3),     "A23"  -> ("ATS-720", 3),    "A24"  -> ("CS-206", 3), 
    "A25"  -> ("CS-206", 3),     "A26"  -> ("CS-206", 3),     "A27"  -> ("CS-223", 3), 
    "A28"  -> ("CS-210", 3),     "A29"  -> ("CS-231", 3),     "A30"  -> ("ATS-720", 3), 
    "A31"  -> ("CS-210", 3),     "A32"  -> ("CS-210", 3),     "A33"  -> ("ATS-100M", 3), 
    "A34"  -> ("ATS-900", 3),    "A35"  -> ("ATS-600", 3),    "A36"  -> ("ATS-600", 3), 
    "A37"  -> ("ATS-720", 3),    "A38"  -> ("ATS-720", 3),    "A39"  -> ("ATS-630J", 3), 
    "A40"  -> ("CAS-3000SA", 3), "A41"  -> ("CAS-3000SA", 3), "A42"  -> ("CAS-3000SA", 3), 
    "A43"  -> ("ATS-110M", 3),   "A44"  -> ("ATS-161H", 3),   "A45"  -> ("CAS-3000SA", 3), 
    "A46"  -> ("ATS-600M", 3),   "A47"  -> ("CAS-3000SA", 3), "A48"  -> ("CAS-4500S", 3), 
    "A49"  -> ("YC-608", 3),     "A50"  -> ("YC-308", 3),     "A51"  -> ("GT-480P", 3), 
    "A52"  -> ("GT-1318P", 3),   "A53"  -> ("GT-1318P", 3),   "A54"  -> ("GT-1318P", 3), 
    "A55"  -> ("GT-1318P", 3),   "A56"  -> ("GT-1318P", 3),   "A57"  -> ("YC-208", 3), 
    "A58"  -> ("GT-480P", 3),    "A59"  -> ("GT-1318P", 3),   "A60"  -> ("GT-1318P", 3), 
    "A61"  -> ("GT-1318P", 3),   "A62"  -> ("GT-1318P", 3),   "A63"  -> ("GT-1318P", 3), 
    "A64"  -> ("GT-1318P", 3),   "A65"  -> ("GT-1318P", 3),   "A66"  -> ("GT-480P", 3), 
    "A67"  -> ("YC-600B", 3),    "B01"  -> ("TAP-306", 5),    "B02"  -> ("TAP-306", 6), 
    "B03"  -> ("TAP-306", 6),    "B04"  -> ("TAP-306", 6),    "B05"  -> ("TAP-306", 6), 
    "B06"  -> ("TAP-306", 6),    "B07"  -> ("TAP-306", 6),    "B08"  -> ("TAP-306", 6), 
    "C01"  -> ("富信成", 5),     "C03"  -> ("富信成", 5),     "C04"  -> ("TAICON自制", 5), 
    "C05"  -> ("TAICON自制", 5), "C06"  -> ("TAICON自制", 5), "C07"  -> ("富信成", 5), 
    "C08"  -> ("富信成", 5),     "E01"  -> ("TSW-100T", 1),   "E03"  -> ("TSW-100T", 1), 
    "E04"  -> ("HSW-800", 1),    "E05"  -> ("TSW-100T", 1),   "E06"  -> ("TSW-100T", 1), 
    "E07"  -> ("TSW-100T", 1),   "E08"  -> ("HSW-800", 1),    "E09"  -> ("TSW-100T", 1), 
    "E10"  -> ("TSW-880B", 1),   "E100" -> ("TSW-100T", 1),   "E101" -> ("ARCO", 2), 
    "E102" -> ("ARCO", 2),       "E104" -> ("HSW-168", 1),    "E106" -> ("HSW-800", 1), 
    "E107" -> ("HSW-160B", 2),   "E108" -> ("HSW-250", 2),    "E11"  -> ("TSW-168T", 2), 
    "E110" -> ("TSW-100T", 1),   "E111" -> ("HSW-160A", 1),   "E112" -> ("HSW-160B", 1), 
    "E113" -> ("TSW-168T", 1),   "E114" -> ("TSW-168T", 2),   "E115" -> ("TSW-168T", 2), 
    "E116" -> ("TSW-168T", 2),   "E117" -> ("TSW-168T", 2),   "E118" -> ("TSW-303", 2), 
    "E119" -> ("TSW-303", 2),    "E12"  -> ("HSW-800", 1),    "E120" -> ("TSW-303", 2), 
    "E121" -> ("TSW-303", 2),    "E122" -> ("TSW-303", 2),    "E123" -> ("TSW-304", 2), 
    "E13"  -> ("HSW-800", 1),    "E15"  -> ("TSW-100T", 1),   "E16"  -> ("TSW-100T", 1), 
    "E17"  -> ("HSW-800", 1),    "E18"  -> ("HSW-160A", 1),   "E19"  -> ("TSW-100T", 1), 
    "E20"  -> ("HSW-160A", 1),   "E21"  -> ("TSW-100T", 1),   "E22"  -> ("HSW-800", 1), 
    "E23"  -> ("TSW-100T", 1),   "E24"  -> ("TSW-100T", 1),   "E25"  -> ("TSW-100T", 1), 
    "E26"  -> ("HSW-500", 1),    "E27"  -> ("HSW-800", 1),    "E28"  -> ("HSW-100T", 1), 
    "E29"  -> ("TSW-100T", 1),   "E30"  -> ("TSW-100T", 1),   "E31"  -> ("TSW-100T", 1), 
    "E32"  -> ("TSW-100T", 1),   "E33"  -> ("TSW-100T", 1),   "E34"  -> ("TSW-100T", 1), 
    "E35"  -> ("TSW-100T", 1),   "E36"  -> ("TSW-100T", 1),   "E37"  -> ("TSW-100T", 1), 
    "E38"  -> ("TSW-100T", 1),   "E39"  -> ("TSW-100T", 1),   "E40"  -> ("HSW-800", 1), 
    "E41"  -> ("TSW-100T", 1),   "E42"  -> ("HSW-800", 1),    "E43"  -> ("TSW-100T", 1), 
    "E44"  -> ("TSW-100T", 1),   "E45"  -> ("TSW-168T", 2),   "E47"  -> ("HSW-250", 2), 
    "E50"  -> ("HSW-800", 1),    "E51"  -> ("TSW-100T", 1),   "E52"  -> ("TSW-168", 2), 
    "E53"  -> ("TSW-168T", 1),   "E54"  -> ("HSW-800", 1),    "E55"  -> ("HSW-800", 1), 
    "E56"  -> ("HSW-800", 1),    "E57"  -> ("TSW-100T", 1),   "E58"  -> ("TSW-100T", 1), 
    "E59"  -> ("TSW-100T", 1),   "E60"  -> ("TSW-100T", 1),   "E61"  -> ("HSW-800", 1), 
    "E62"  -> ("TSW-168T", 1),   "E63"  -> ("TSW-100T", 1),   "E64"  -> ("HSW-100T", 1), 
    "E65"  -> ("TSW-800", 1),    "E66"  -> ("HSW-800", 1),    "E67"  -> ("HSW-168", 1), 
    "E68"  -> ("HSW-800", 1),    "E69"  -> ("HSW-800", 1),    "E70"  -> ("TSW-100T", 1), 
    "E71"  -> ("HSW-800", 1),    "E72"  -> ("TSW-168T", 1),   "E73"  -> ("HSW-500", 1), 
    "E74"  -> ("HSW-800", 1),    "E75"  -> ("TSW-100T", 1),   "E76"  -> ("HSW-800", 1), 
    "E77"  -> ("HSW-800", 1),    "E78"  -> ("TSW-100T", 1),   "E79"  -> ("HSW-800", 1), 
    "E80"  -> ("HSW-800", 1),    "E81"  -> ("TSW-100T", 1),   "E82"  -> ("TSW-100T", 1), 
    "E83"  -> ("TSW-100T", 1),   "E84"  -> ("HSW-880B", 1),   "E85"  -> ("TSW-100T", 1), 
    "E86"  -> ("HSW-100T", 1),   "E87"  -> ("HSW-160B", 1),   "E88"  -> ("HSW-160B", 2), 
    "E90"  -> ("HSW-250", 1),    "E91"  -> ("TSW-100T", 1),   "E92"  -> ("TSW-100T", 1), 
    "E99"  -> ("TSW-100T", 1),   "G01"  -> ("FTO-2200A", 2),  "G03"  -> ("FTO-2200", 2), 
    "G04"  -> ("FTO-2200A", 2),  "G05"  -> ("FTO-2200A", 2),  "G06"  -> ("FTO-2200A", 2), 
    "G07"  -> ("FTO-2200A", 2),  "G08"  -> ("FTO-2400", 2),   "G09"  -> ("FTO-2200A", 2), 
    "G10"  -> ("FTO-2200A", 2),  "G100" -> ("FTO-2200A", 2),  "G103" -> ("SBB-1800", 2), 
    "G104" -> ("FTO-2700", 2),   "G105" -> ("FTO-3100", 2),   "G106" -> ("SAA-1800", 2), 
    "G107" -> ("3050", 2),       "G108" -> ("3050", 2),       "G109" -> ("3050", 2), 
    "G110" -> ("TIA-2600", 2),   "G111" -> ("富信成", 2),     "G12"  -> ("FTO-2400", 2), 
    "G13"  -> ("FTO-2500", 2),   "G15"  -> ("FTO-2200A", 2),  "G16"  -> ("FTO-2200A", 2), 
    "G17"  -> ("FTO-2500", 2),   "G18"  -> ("FTO-3100", 2),   "G19"  -> ("FTO-2700", 2), 
    "G20"  -> ("FTO-2700", 2),   "G21"  -> ("FTO-2200A", 2),  "G22"  -> ("FTO-2200", 2), 
    "G23"  -> ("FTO-2400", 2),   "G24"  -> ("FTO-2200", 2),   "G25"  -> ("FTO-2200", 2), 
    "G26"  -> ("FTO-2200A", 2),  "G27"  -> ("FTO-2400", 2),   "G28"  -> ("FTO-2700", 2), 
    "G29"  -> ("FTO-2600", 2),   "G30"  -> ("FTO-2200A", 2),  "G31"  -> ("FTO-2400", 2), 
    "G32"  -> ("FTO-2500", 2),   "G33"  -> ("FTO-2200A", 2),  "G34"  -> ("FTO-2200A", 2), 
    "G35"  -> ("FTO-2200A", 2),  "G36"  -> ("FTO-2200A", 2),  "G37"  -> ("FTO-2200A", 2), 
    "G38"  -> ("FTO-2200A", 2),  "G39"  -> ("FTO-2400", 2),   "G40"  -> ("FTO-2400", 2), 
    "G41"  -> ("FTO-2200A", 2),  "G42"  -> ("FTO-2200A", 2),  "G43"  -> ("FTO-2200A", 2), 
    "G44"  -> ("FTO-2200A", 2),  "G45"  -> ("FTO-2200A", 2),  "G46"  -> ("FTO-2200A", 2), 
    "G47"  -> ("FTO-2200A", 2),  "G49"  -> ("SBB-1800", 2),   "G50"  -> ("TIA-2600", 2), 
    "G51"  -> ("TIA-2600", 2),   "G52"  -> ("SBB-1800", 2),   "G53"  -> ("TIA-2600", 2), 
    "G54"  -> ("FTO-2400", 2),   "G55"  -> ("FTO-2500", 2),   "G56"  -> ("FTO-2400", 2), 
    "G57"  -> ("FTO-2500", 2),   "G58"  -> ("TIA-2600", 2),   "G59"  -> ("FTO-2500", 2), 
    "G60"  -> ("FTO-2400", 2),   "G61"  -> ("FTO-2400", 2),   "G62"  -> ("TIA-2600", 2), 
    "G63"  -> ("FTO-2200A", 2),  "G64"  -> ("FTO-2500", 2),   "G65"  -> ("FTO-2200A", 2), 
    "G66"  -> ("FTO-2400", 2),   "G67"  -> ("FTO-2510", 2),   "G68"  -> ("FTO-2500", 2), 
    "G69"  -> ("FTO-2500", 2),   "G70"  -> ("FTO-2200A", 2),  "G71"  -> ("FTO-2500", 2), 
    "G72"  -> ("TIA-2600", 2),   "G73"  -> ("FTO-3010", 2),   "G74"  -> ("FTO-2500", 2), 
    "G75"  -> ("FTO-2400", 2),   "G76"  -> ("FTO-2500", 2),   "G77"  -> ("FTO-2500", 2), 
    "G78"  -> ("FTO-2200A", 2),  "G79"  -> ("FTO-2200A", 2),  "G80"  -> ("FTO-2500", 2), 
    "G81"  -> ("FTO-3010", 2),   "G82"  -> ("FTO-3000", 2),   "G83"  -> ("TIA-2600", 2), 
    "G84"  -> ("FTO-3010", 2),   "G85"  -> ("FTO-2500", 2),   "G86"  -> ("FTO-2500", 2), 
    "G91"  -> ("TIA-2600", 2),   "G92"  -> ("TIA-2600", 2),   "G93"  -> ("FTO-2200A", 2), 
    "G94"  -> ("FTO-3100", 2),   "G95"  -> ("FTO-3100", 2),   "G96"  -> ("FTO-3100", 2), 
    "G97"  -> ("FTO-3100", 2),   "G98"  -> ("FTO-3100", 2),   "G99"  -> ("FTO-2200A", 2), 
    "K1"   -> ("KAIDO", 2),      "K2"   -> ("KAIDO", 2),      "S1"   -> ("SPH-3000", 2), 
    "S2"   -> ("SPH-3000", 2),   "S3"   -> ("SPH-3000", 2),   "S4"   -> ("SPH-3000", 2), 
    "S5"   -> ("SPH-3000", 2),   "S6"   -> ("SPH-3000", 2),   "S7"   -> ("SPH-3000", 2), 
    "S8"   -> ("SPH-3000", 2),   "T01"  -> ("NCR236A", 4),    "T02"  -> ("NCR236A", 4), 
    "T03"  -> ("NCR236A", 4),    "T04"  -> ("NCR236A", 4),    "T05"  -> ("NCR236A", 4), 
    "T06"  -> ("NCR236A", 4),    "T07"  -> ("NCR236A", 4),    "T08"  -> ("NCR236A", 4), 
    "T09"  -> ("NCR-356B", 4),   "T10"  -> ("NCR236A", 4),    "T11"  -> ("NCR236A", 4), 
    "T12"  -> ("NCR236A", 4),    "T13"  -> ("NCR236A", 4),    "T16"  -> ("NCR236A", 4), 
    "T17"  -> ("NCR236A", 4),    "T18"  -> ("JET250E", 4),    "T19"  -> ("JET-250E", 4), 
    "T20"  -> ("NCR236A", 4),    "T21"  -> ("NCR356B", 4),    "T22"  -> ("CFT450E", 4), 
    "T23"  -> ("HTP-300", 4),    

    "E49-K1" -> ("KAIDO", 2),    "E49-K2" -> ("KAIDO", 2),    "E49-S1" -> ("SPH-3000", 2),    
    "E49-S2" -> ("SPH-3000", 2), "E49-S3" -> ("SPH-3000", 2), "E49-S4" -> ("SPH-3000", 2), 
    "E49-S5" -> ("SPH-3000", 2), "E49-S6" -> ("SPH-3000", 2), "E49-S7" -> ("SPH-3000", 2), 
    "E49-S8" -> ("SPH-3000", 2)
  )

  def getProduct(machineID: String) = productMapping.get(machineID).getOrElse("Unknow")
  def getModel(machineID: String) = machineModel.get(machineID).map(_._1).getOrElse("Unknown")
  def getMachineType(machineID: String) = {
    for {
      machModel <- machineModel.get(machineID).map(_._2)
      machType  <- machineType.get(machModel)
    } yield machType
  }.getOrElse("Unknown")

}

