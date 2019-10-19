type ('nonterminal, 'terminal) symbol =
	| N of 'nonterminal
	| T of 'terminal

let rec get_rules nonterminal rules =
	match rules with
	| [] -> []
	| (nt, symbols)::rem_rules -> 
		if nonterminal == nt 
		then symbols::(get_rules nonterminal rem_rules)
		else get_rules nonterminal rem_rules
;;

let rule_homomorphism gram1 nonterminal = get_rules nonterminal (snd gram1);;

let convert_grammar gram1 =
	fst gram1, (function nonterminal -> rule_homomorphism gram1 nonterminal)
;;

let rec matcher rule_homomorphism root current_rules acceptor derivation fragment =
	
	let rec prefix_parse rule_homomorphism current_production acceptor derivation fragment =
		match current_production with
		| [] -> acceptor derivation fragment
		| N nonterminal::rest_of_production -> matcher rule_homomorphism nonterminal (rule_homomorphism nonterminal) (prefix_parse rule_homomorphism rest_of_production acceptor) derivation fragment 
		| T terminal::rest_of_production -> 
			match fragment with
			| [] -> None 
			| head_terminal::remaining_fragment ->
				if head_terminal = terminal
				then prefix_parse rule_homomorphism rest_of_production acceptor derivation remaining_fragment
				else None 
	in

	let rec concatenator rule_homomorphism root current_rules acceptor derivation fragment =
		match current_rules with
		| [] -> None
		| rule::remaining_rules -> 
			match prefix_parse rule_homomorphism rule acceptor (derivation@[(root, rule)]) fragment with
			| None -> concatenator rule_homomorphism root remaining_rules acceptor derivation fragment
			| Some x -> Some x
	in

	concatenator rule_homomorphism root current_rules acceptor derivation fragment
;;

let parse_prefix grammar acceptor fragment =
	matcher (snd grammar) (fst grammar) ((snd grammar) (fst grammar)) acceptor [] fragment;;
