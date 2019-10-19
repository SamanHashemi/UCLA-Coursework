type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal

let rec match_symbol cur_sym accept current_match frag = function
		| [] -> None 
		| cur_rule::rest_of_rules ->
			(function
			| None -> match_symbol cur_sym accept current_match frag rest_of_rules
			| ret -> ret)
			 (let cand_derivation = current_match @ [cur_sym, cur_rule] 
			 ;;

let convert_grammar gram1 =
	let rec get_rules_for_symbol rules symbol = match rules with
		| [] -> []
		| h::t when (fst h) = symbol -> (snd h)::(get_rules_for_symbol t symbol)
		| h::t -> get_rules_for_symbol t symbol in 
	(fst gram1), (fun symbol -> get_rules_for_symbol (snd gram1) symbol);;

let make_matcher gram accept frag = 
	let rule_func = snd gram in 
	produces_valid_match cur_rule accept cand_derivation frag) 
	and produces_valid_match cur_rule accept current_match frag = 
	match cur_rule with
		| [] -> accept current_match frag 
		| _ -> match frag with
			| [] -> None
			| prefix::suffix -> match cur_rule with 
				| (T term_sym)::t when prefix = term_sym -> produces_valid_match t accept current_match suffix
				| (T term_sym)::t -> None 
				| (N nonterm_sym)::t -> (fun rhs_rule_acceptor -> 
					match_symbol nonterm_sym rhs_rule_acceptor current_match frag (rule_func nonterm_sym))
					 (produces_valid_match t accept)
				| [] -> None in

	let start_symbol = fst gram and rules_for_start = rule_func (fst gram) in 
	match_symbol start_symbol accept [] frag rules_for_start;;