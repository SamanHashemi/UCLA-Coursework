rotate(Matrix, RotatedMatrix) :-
	transpose_matrix(Matrix, T),
	reverse_rows(T, RotatedMatrix).

reverse_rows([], []).
reverse_rows([R|Rem], [Rev|RemRevs]) :-
	reverse(R, Rev),
	reverse_rows(Rem, RemRevs).

transpose_matrix([], []).
transpose_matrix([R|Rem], T) :-
	transpose_matrix(R, [R|Rem], T).

transpose_matrix([], _, []).
transpose_matrix([_|Rem], CurM, [TRow|RemTrans]) :-
	lists_firsts_rests(CurM, TRow, NewMatrix),
	transpose_matrix(Rem, NewMatrix, RemTrans).

lists_firsts_rests([], [], []).
lists_firsts_rests([[F|Os]|Rest], [F|Fs], [Os|Oss]) :-
        lists_firsts_rests(Rest, Fs, Oss).

unique_mat(RightM, R, D, L, U) :-
	right_rules(RightM, R),
	rotate(RightM, UpMat),
	right_rules(UpMat, U),
	rotate(UpMat, LeftMat),
	right_rules(LeftMat, L),
	rotate(LeftMat, DownMat),
	right_rules(DownMat, D).

corr_vals([], _).
corr_vals([Cur|Rem], N) :-
	Cur #< N,
	corr_vals(Rem, N).

right_rules([], []).
right_rules([R|Rem], [RRule|RemRules]) :-
	right_rules(Rem, RemRules),
	corr_rule(R, RRule).

corr_rule([], F) :- 
	F = 0.
corr_rule([H|T], F) :-
	\+ corr_vals(T, H),
	corr_rule(T, F).
corr_rule([H|T], F) :-
	corr_vals(T, H),
	corr_rule(T, X),
	F is X + 1.


check_lens(U, D, L, R, N) :-
	length(U, N), length(D, N), length(R, N), length(L, N).


row_len([], _).
row_len([R|Rem], N) :-
	length(R, N),
	row_len(Rem, N).


plain_dom_int(N, X) :-
	my_domain(X, 1, N).


dom_int(N, X) :-
	fd_domain(X, 1, N).


my_domain([], _, _).
my_domain([Cur|Rem], Start, End) :-
	Cur #>= Start,
	Cur #=< End,
	my_domain(Rem, Start, End).


my_label(N, L) :-
	findall(Num, between(1, N, Num), X), 
	permutation(X, L).


all_diff_check([]).
all_diff_check([H|Rem]) :-
	not_in_list(H, Rem),
	all_diff_check(Rem).


not_in_list(_, []).
not_in_list(Cur, [H|Rem]) :-
	Cur #\= H,
	not_in_list(Cur, Rem).


prevent0(T0,T1,T):-
    T0 == T1,
    T is 1;
    T0 \= T1,
    T is T1-T0.


tower(N, T, C) :-
	length(T, N),
	row_len(T, N),
	C = counts(U, D, L, R),
	check_lens(U, D, L, R, N),
	maplist(dom_int(N), T),
	maplist(fd_all_different, T),
	rotate(T, Rot),
	maplist(dom_int(N), Rot),
	maplist(fd_all_different, Rot),
	maplist(fd_labeling, T),
	reverse(RevL, L),
	reverse(RevD, D),
	unique_mat(T, R, RevD, RevL, U).


plain_tower(N, T, C) :-
	length(T, N),
	row_len(T, N),
	C = counts(U, D, L, R),
	check_lens(U, D, L, R, N),
	maplist(plain_dom_int(N), T),
	maplist(all_diff_check, T),
	rotate(T, Rot),
	maplist(dom_int(N), Rot),
	maplist(all_diff_check, Rot),
	maplist(my_label(N), T),
	reverse(RevL, L),
	reverse(RevD, D), 
	unique_mat(T, R, RevD, RevL, U).


speedup(Ratio) :-
	% Get tower time
	statistics(cpu_time,[T0,_]),
    tower(5, _, counts([2,1,3,3,2],[3,4,3,2,1],[2,1,2,4,4],[2,3,3,2,1])),
    statistics(cpu_time,[T1,_]),

    % Get plain tower time
    plain_tower(5, _, counts([2,1,3,3,2],[3,4,3,2,1],[2,1,2,4,4],[2,3,3,2,1])),
    statistics(cpu_time,[T2,_]),
	prevent0(T0,T1,T),
	Ratio is (T2-T1)/T.


ambiguous(N, C, T1, T2) :-
	C = counts(_, _, _, _),
	tower(N, T1, C),
	tower(N, T2, C),
	T1 \= T2.
