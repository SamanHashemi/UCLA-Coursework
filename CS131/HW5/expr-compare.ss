(define (cons* a b) (cons a b))

(define (self-return a) a)

(define (bind a b)
	(string->symbol
		(string-append
			(symbol->string a) "!" (symbol->string b)
		)
	)
)

(define (replace-all v l) (cond
	([empty? l] '())
	([equal? (type-check l) 'lambda] (cons (car l) (replace-all v (cdr l))))
	([not (pair? (car l))] (cons (v (car l)) (replace-all v (cdr l))))
	(else (cons (replace-all v (car l)) (replace-all v (cdr l))))
))

(define (type-check x)
	(if [not (pair? x)] 'base
	(let ([hd (car x)]) (cond
		([and (pair? hd)
			(or (equal? (car hd) 'lambda) (equal? (car hd) (string->symbol "\u03BB")))] 'lambda)
		([equal? hd 'if] 'if)
		([equal? hd 'quote] 'base)
		(else 'list)
	))
))


(define (expr-compare-lambda cur_a cur_b ali bli) (cond
	([not (empty? ali)]
		(if [not (equal? (car ali) (car bli))]
			(let
				([new_a (lambda (x) 
					(cond ([not (equal? x (car ali))] (cur_a x)) (else (bind (car ali) (car bli)))))]
				[new_b (lambda (y) 
					(cond ([not (equal? y (car bli))] (cur_b y)) (else (bind (car ali) (car bli)))))])
				(cons
					(bind (car ali) (car bli))
					(expr-compare-lambda new_a new_b (cdr ali) (cdr bli)) 
				)
			)
			(cons (car ali) (expr-compare-lambda cur_a cur_b (cdr ali) (cdr bli))
			)			
		)
	)
	(else
		(list cur_a cur_b)
	)
))

(define (expr-compare a b) (let ([atype (type-check a)] [btype (type-check b)]) (cond
	([equal? atype 'quote] (cond
	([and (equal? a #f) (equal? b #t)] '(not %))
	([and (equal? a #t) (equal? b #f)] '%)
	([equal? a b] a)
	(else (list 'if '% a b))))

	([equal? atype 'base] (cond
	([and (equal? a #f) (equal? b #t)] '(not %))
	([and (equal? a #t) (equal? b #f)] '%)
	([equal? a b] a)
	(else (list 'if '% a b))))

	([equal? atype 'lambda]
		(cons (let ([c (cdr (car a))] [d (cdr (car b))] 
			[lambda_new (if (not (equal? (car (car a)) (car (car b)))) 
			(string->symbol "\u03BB") (car (car a)))])
		(let* ([ret (expr-compare-lambda self-return self-return (car c) (car d))] 
				[r (take-right ret 2)] [res (drop-right ret 2)]
				[ail (replace-all (first r) (cdr c))]
				[bil (replace-all (second r) (cdr d))])
				(cons* lambda_new (cons* res (expr-compare ail bil))))) 
		(expr-compare (cdr a) (cdr b))))

	([not (equal? atype btype)] (list 'if '% a b))
	([not (equal? (length a) (length b))] (list 'if '% a b))
	
	(else (cons (expr-compare (car a) (car b)) (expr-compare (cdr a) (cdr b))))
)))


(define (test-expr-compare x y)
  (and (equal? (eval x) (eval (list 'let '((% #t)) (expr-compare x y))))
       (equal? (eval y) (eval (list 'let '((% #f)) (expr-compare x y))))
))

(define test-expr-x '(+ 3 ((lambda (a b) (list a b)) 1 2)))
(define test-expr-y '(+ 2 ((lambda (a c) (list a c)) 1 2)))