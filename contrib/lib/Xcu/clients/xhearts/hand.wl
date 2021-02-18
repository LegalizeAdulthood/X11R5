
XcuDeck
  {
  name "table"
  facetWidth "0"
  XcuTbl
    {
    name "exchange"
    formatString "c\nc c c."
    resizeParticipants "children"
    *Tbl.background "gray90"
    background "gray90"
    *Tbl.borderColor "gray90"
    internalHeight "20"
    internalWidth "20"
    interWidth "10"
    interHeight "5"
    equalColumns "True"
    facetWidth "5"

    *Command.borderColor "Black"
    *Command.background "gray90"
    *biggestLabel "W"
    *facetWidth "0"
    XcuDeck
      {
      name "exchange"
      *facetWidth "3"
      *biggestLabel "(Click to get)\nx\nx"
      XcuLabel { name "discard" label "To Discard" }
      XcuLabel { name "waiting" label  "Waiting for\nfoobar's\ndiscards" }
      XcuCommand { name "confirm_discard" label "Click\nto\nConfirm" }
      XcuCommand { name "confirm_receipt"
		 label "Received from\nfoobar\n(Click to get)" }
      }
    XcuTbl
	{
	XcuCommand { name "discard0"  gravity "e" label " " }
	XcuDeck
	    {
	    name "discard0"
	    Label { gravity "w" name "Spades0"
		       foreground "Black" bitmap "spade.xbm" }
	    Label { gravity "w" name "Hearts0"
		       foreground "Red" bitmap "heart.xbm" }
	    Label { gravity "w" name "Diamonds0"
		       foreground "Red" bitmap "diamond.xbm" }
	    Label { gravity "w" name "Clubs0"
		       foreground "Black" bitmap "club.xbm" }
	    XcuLabel { gravity "w" name "Blank0" label " " }
	    }
	}
    XcuTbl
	{
	XcuCommand { name "discard1"  gravity "e" label " " }
	XcuDeck
	    {
	    name "discard1"
	    Label { gravity "w" name "Spades1"
		       foreground "Black" bitmap "spade.xbm" }
	    Label { gravity "w" name "Hearts1"
		       foreground "Red" bitmap "heart.xbm" }
	    Label { gravity "w" name "Diamonds1"
		       foreground "Red" bitmap "diamond.xbm" }
	    Label { gravity "w" name "Clubs1"
		       foreground "Black" bitmap "club.xbm" }
	    XcuLabel { gravity "w" name "Blank1" label " " }
	    }
	}
    XcuTbl
	{
	XcuCommand { name "discard2"  gravity "e" label " " }
	XcuDeck
	    {
	    name "discard2"
	    Label { gravity "w" name "Spades2"
		       foreground "Black" bitmap "spade.xbm" }
	    Label { gravity "w" name "Hearts2"
		       foreground "Red" bitmap "heart.xbm" }
	    Label { gravity "w" name "Diamonds2"
		       foreground "Red" bitmap "diamond.xbm" }
	    Label { gravity "w" name "Clubs2"
		       foreground "Black" bitmap "club.xbm" }
	    XcuLabel { gravity "w" name "Blank2" label " " }
	    }
	}
    /***
    XcuCommand { name "discard0"  label " " }
    XcuCommand { name "discard1"  label " " }
    XcuCommand { name "discard2"  label " " }
    ***/
    }
  XcuTbl
    {
    name "play"
    formatString "c\nc c c\nc."
    resizeParticipants "children"
    background "PaleGreen"
    *Tbl.borderColor "PaleGreen"
    *Tbl.equalColumns "True"
    equalColumns "True"
    internalHeight "20"
    internalWidth "20"
    *facetWidth "0"
    facetWidth "5"

    *borderColor "PaleGreen"
    *background "PaleGreen"
    *Label.gravity "w"
    /*****
    *resizeParticipants "internals"
    ****/
    XcuTbl
	{
	XcuLabel { gravity "e" label " " }
	XcuLabel { gravity "e" label " " }
	XcuLabel { gravity "e" name "north_card"  label " " }
	XcuDeck
	    {
	    name "north"
	    XcuLabel { gravity "w" name "north_Blank"  label "  " }
	    Label { gravity "w" name "north_Spades"
		       foreground "Black" bitmap "spade.xbm" }
	    Label { gravity "w" name "north_Hearts"
		       foreground "Red" bitmap "heart.xbm" }
	    Label { gravity "w" name "north_Diamonds"
		       foreground "Red" bitmap "diamond.xbm" }
	    Label { gravity "w" name "north_Clubs"
		       foreground "Black" bitmap "club.xbm" }
	    }
	XcuLabel { gravity "e" label " " }
	XcuLabel { gravity "e" label " " }
	}
    XcuTbl
	{
	XcuLabel { gravity "e" name "west_card"  label "  " }
	XcuDeck
	    {
	    name "west"
	    XcuLabel { gravity "w" name "west_Blank"  label " " }
	    Label { gravity "w" name "west_Spades"
		       foreground "Black" bitmap "spade.xbm" }
	    Label { gravity "w" name "west_Hearts"
		       foreground "Red" bitmap "heart.xbm" }
	    Label { gravity "w" name "west_Diamonds"
		       foreground "Red" bitmap "diamond.xbm" }
	    Label { gravity "w" name "west_Clubs"
		       foreground "Black" bitmap "club.xbm" }
	    }
	}
    XcuLabel { name "middle" label "  " }
    XcuTbl
	{
	XcuLabel { gravity "e" name "east_card"  label " " }
	XcuDeck
	    {
	    name "east"
	    XcuLabel { gravity "w" name "east_Blank"  label " " }
	    Label { gravity "w" name "east_Spades"
		       foreground "Black" bitmap "spade.xbm" }
	    Label { gravity "w" name "east_Hearts"
		       foreground "Red" bitmap "heart.xbm" }
	    Label { gravity "w" name "east_Diamonds"
		       foreground "Red" bitmap "diamond.xbm" }
	    Label { gravity "w" name "east_Clubs"
		       foreground "Black" bitmap "club.xbm" }
	    }
	}
    XcuTbl
	{
	XcuLabel { gravity "e" label " " }
	XcuLabel { gravity "e" label " " }
	XcuLabel { gravity "e" name "south_card"  label " " }
	XcuDeck
	    {
	    name "south"
	    XcuLabel { gravity "w" name "south_Blank"  label " " }
	    Label { gravity "w" name "south_Spades"
		       foreground "Black" bitmap "spade.xbm" }
	    Label { gravity "w" name "south_Hearts"
		       foreground "Red" bitmap "heart.xbm" }
	    Label { gravity "w" name "south_Diamonds"
		       foreground "Red" bitmap "diamond.xbm" }
	    Label { gravity "w" name "south_Clubs"
		       foreground "Black" bitmap "club.xbm" }
	    }
	XcuLabel { gravity "e" label " " }
	XcuLabel { gravity "e" label " " }
	}
    /****
    XcuLabel { gravity "c" name "north_card"  label " " }
    XcuLabel { gravity "w" name "west_card"  label " " }
    XcuLabel { gravity "e" name "east_card"  label " " }
    XcuLabel { gravity "c" name "south_card"  label " " }
    ****/
    }
  XcuTbl
    {
    name "moon_decision"
    formatString "c."
    *facetWidth "5"
    XcuCommand { name "lower_me"      label "Reduce my score" }
    XcuCommand { name "raise_others"  label "Increase others" }
    }
  XcuTbl
    {
    name "hand_decision"
    formatString "c."
    *facetWidth "5"
    XcuCommand { name "another_hand"    label "Another Hand" }
    XcuCommand { name "no_more"  	label "I Quit" }
    }
  }


