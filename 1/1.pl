use 5.010;
$\=$/;
@Memory = ();
$WireCount = 23;
@wires = ();
@wires_mask = ();

open (MEMORY, "input.txt");
$i=0;
$Memory[$i++]=$_ while (chomp($_=<MEMORY>));
close MEMORY;
open (CONFIG, "config.txt");
read (CONFIG,$wires_mask[$_],1) for (0..22);
close CONFIG;
$wires[$_] = 0 for (0..22);

			while(1)
			{
				MemoryPointer();
				system(pause);
				CommandRegister();
				Plus();
				MemoryPointer();
				CommandDecoder();
				ALU();
				IndexRegister();
				GeneralPurposeRegister();
				IORegister();
				CommandPointer();
				if ($wires[1] * $wires_mask[1] == 0)
					{return 0};
			}


        sub CommandPointer() 
        {
            $wires[10] = Multyplexor($wires[9] * $wires_mask[9], $wires[10] * $wires_mask[10] + 1, $wires[14] * $wires_mask[14], 0);
        }
		
       sub CommandRegister() 
        {
            $com = $wires[11] * $wires_mask[11];
            $wires[13] = $com % 256;
            $wires[12] = $com / 256;
        }

        sub CommandDecoder()
        {
            $cop = $wires[12] * $wires_mask[12];
            $p = 0;
            $i = 0;
            given ($cop)
            {
                when (0){ $p = 0;}
                when (0x11){ $i = 0; $p = 1; }
                when (0x15){ $i = 1; $p = 1;} 
                when (0x02){ $p = 2;}
                when (0x21){ $i = 0; $p = 1;}
                when (0x25){ $i = 1; $p = 1;}
                when (0x31){ $i = 0; $p = 1;}
                default {$p = 4;}                 
            }
            $wires[1] = (($wires[12]*$wires_mask[12])  != 0xFF)?1:0;
            $wires[2] = ($p == 3)?1:0;
            $wires[3] = ($p == 1)?1:0;
            $wires[4] = ($p != 3)?1:0;
            $wires[5] = (!($p == 2 || $p == 3));
            $wires[6] = $cop /16;
            $wires[7] = $i;
            $wires[8] = (p == 0)?1:0;
            if (($cop /16) == 0xF)
            {
                if (
                    ($cop == 0xF0 && (($wires[21] * $wires[21]) == 0))
                    || ($cop == 0xF1 && ($wires[21] * $wires_mask[21] > 0))
                    || ($cop == 0xF4 && ($wires[19] * $wires_mask[19] == 0))
                    || ($cop == 0xF5 && ($wires[19] * $wires_mask[19] == 1))
                    ){
                    $wires[9] = 1;}
                else {$wires[9] = 0;}
            }
        }
		
        sub Multyplexor#(int f,int w1, int w2, int w3)
        {
			return @_[@_[0]];
        }

		sub Plus() 
        { 
            $wires[14] = ($wires[22] * $wires_mask[22] + $wires[13] * $wires_mask[13])%256;
        }
		
        sub ALU() 
        {
            given ($wires[6] * $wires_mask[6]) 
            {
                when(0) 
                    { 
                        $wires[16]=$wires[20]*$wires_mask[20];
                        $wires[17]=($wires[16]>0)?1:0;
                    }
                when(1)
                    {
                        $wires[16]=Multyplexor($wires[7]*$wires_mask[7],$wires[15]*$wires_mask[15],$wires[14]*$wires_mask[14],$wires[20]*$wires_mask[20]);
                        $wires[17]=($wires[16]>0)?1:0;
                    }
				when(2)
					{
                        $wires[16]=0xFFFF&($wires[20]*$wires_mask[20]+Multyplexor($wires[7]*$wires_mask[7],$wires[15]*$wires_mask[15],$wires[14]*$wires_mask[14],$wires[20]*$wires_mask[20]));
                        $wires[17]=($wires[16]>0)?1:0;
                    }
                when(3)
                    {
                        $wires[16] = 0xFFFF & ($wires[20] * $wires_mask[20] - Multyplexor($wires[7] * $wires_mask[7], $wires[15] * $wires_mask[15], $wires[14] * $wires_mask[14], $wires[20] * $wires_mask[20]));
                        $wires[17]=($wires[16]>0)?1:0;
                    }
            }
        }
		
        sub IndexRegister() 
        {
            $wires[22] = Multyplexor($wires[5] * $wires_mask[5], $wires[16] * $wires_mask[16], 0, 0) if ($wires[4] * $wires_mask[4] != 0) ;
        }
		
        sub GeneralPurposeRegister() 
        {
            if ($wires[3] * $wires_mask[3] != 0) 
            {
                $wires[20] = $wires[16] * $wires_mask[16];
                $wires[21] = $wires[17] * $wires_mask[17];
            }
        }
		
        sub  IORegister()
        {
            $wires[19] = 1;
        }
		
        sub MemoryPointer()
        {
            print $wires[11] = hex($Memory[$wires[10]*$wires_mask[10]]);
            print $wires[15] = hex($Memory[$wires[14]*$wires_mask[14]]);
        }