type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal


(* converts a hw1 grammar to a hw2 grammar.
We basically have a recursive function get_rules_for_symbol that returns a list of lists representing
the rules for that symbol. Our overall return is a tuple with the start element and a function that basically
calls the recursive function get_rules_for_symbol *)
let convert_grammar gram1 =
	let rec get_rules_for_symbol rules symbol = match rules with
		| [] -> []
		| h::t when (fst h) = symbol -> (snd h)::(get_rules_for_symbol t symbol)
		| h::t -> get_rules_for_symbol t symbol in 
	(fst gram1), (fun symbol -> get_rules_for_symbol (snd gram1) symbol);;

let parse_prefix gram accept frag = 
	let rule_func = snd gram in (* store the rule getter which we query with a symbol *)
	(* match symbol goes through rules for the current symbol. We try to make a derivation, query the matcher to see if it is valid,
	and if not, we backtrack - unmake the derivation and try for the next rule. If we run out of rules we could not do the parse
	so we return None*)
	let rec match_symbol cur_sym accept current_match frag = function
		(* shorthand, we are matching the rules *)
		| [] -> None (* no more rules to match *)
		(* naive parsing - try the current rule and see if we can make a match, if yes then just return that. otherwise backtrack. *)
		| cur_rule::rest_of_rules ->
			(function
				(* match failed - undo the choice and backtrack to try the next rules *)
			| None -> match_symbol cur_sym accept current_match frag rest_of_rules (* we can't match with this rule. Go to the next rule and try it. *)
			(* the call to produces_valid_match was successful and should've returned what the new acceptor returned, so return that  *)
			| ret -> ret)
			 (let cand_derivation = current_match @ [cur_sym, cur_rule] 
			 in produces_valid_match cur_rule accept cand_derivation frag) (* run the above anonymous function with the result of produces_valid_match *)
(* produces_valid_match is the matcher - it tries to match the current rule with a prefix of the fragment. If we have no more rules, then we've generated a candidate parse,
and we simply run the acceptor. Otherwise, we seee if a prefix corresponds to a terminal symbol corresponding to the current rule. If so, we 
recursively call this function on the rest of the rules and the suffix of the fragment.
If it corresponds to a nonterminal symbol, we have to pass in a new acceptor that runs on the remaining rules
and then call match_symbol with that new acceptor *) 
	and produces_valid_match cur_rule accept current_match frag = 
	match cur_rule with
		| [] -> accept current_match frag (* run the acceptor on the current match if there are no more rules to parse *)
		| _ -> match frag with
			| [] -> None
			| prefix::suffix -> match cur_rule with 
			(* if the prefix of the fragment is a terminal symbol, we can continue to match on the rest of the rule *)
				| (T term_sym)::t when prefix = term_sym -> produces_valid_match t accept current_match suffix
				| (T term_sym)::t -> None (* have to backtrack since its a terminal symbol but doesnt match the current prefix *)
				(* otherwise we have a nonterminal symbol. We try to match_symbol with this new nonterminal symbol, but we need a new acceptor
				that is made from the rest of the rules (t). We create that acceptor from the rest of the rules and use it instead of the old acceptor
				as input to match symbol. Then match symbol will try to match the rest of the rules with this new nonterminal and new acceptor. *)
				| (N nonterm_sym)::t -> (fun rhs_rule_acceptor -> 
					match_symbol nonterm_sym rhs_rule_acceptor current_match frag (rule_func nonterm_sym))
					 (produces_valid_match t accept) (* curried the produced_valid_match to make acceptor *)
				| [] -> None in

	let start_symbol = fst gram and rules_for_start = rule_func (fst gram) in 
	match_symbol start_symbol accept [] frag rules_for_start;;