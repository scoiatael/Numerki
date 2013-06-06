import Data.List (unfoldr)

--metoda :: (Float -> Float) -> [Float]


falsi g a b = iterate (\(x,y) -> falaux g (x,y)) (a,b)
falaux g (x0,x1) = let gx0 = g(x0) in let gx1 = g x1 in let c = (gx1*x0 - gx0*x1)/(gx1-gx0) in let gxc = g(c) in if gx0*gxc < 0 then (x0,c) else (c,x1)

newton g gp a = iterate (\x -> x-(g x/gp x)) a
upnewton g gp0 a = iterate (\x -> x-(g x/gp0)) a
siecznych g a b = iterate (\(x,y) ->  (x- (g x) * (x-y)/(g x - g y),x)) (a,b) 

f x = (cos(2*x))**2-x**2
fp x = -2*sin(4*x) - 2*x 
  
alpha = 0.5149332647 

zadnew = newton f fp 0.75
zadupnew = let x0 = 0.75 in upnewton f (fp x0) x0
zadsiecz = map (\(a,b) -> (a+b)/2) $ siecznych f 0.75 0.0
zadfalsi = map (\(a,b) -> (a+b)/2) $ falsi f 0 0.75

paruj xs = zipWith (,) xs (tail xs)

znew = paruj zadnew
zupnew = paruj zadupnew
zsiecz = paruj zadsiecz
zfalsi = paruj zadfalsi

auxf (x,y) = (log $  abs(x-y))/(log 10)
zip4 (x:xs) (y:ys) (w:ws) (z:zs) = [x,y,w,z]:(zip4 xs ys ws zs)
lista =  ( map (map (auxf)) (zip4 znew zupnew zsiecz zfalsi))
zaux = zipWith (++) (map (:[]) [0..]) (takeWhile (\xs -> or (map (>(-20)) xs)) $ lista)

niceformat xs = 
	putStr $ map (\x -> if x==',' then ' ' else x) $
	 concatMap (\x -> if or[(x==')'), (x=='('), (x=='['), (x==']')] then [] else [x]) $ 
     concatMap (\x -> (show x) ++ "\n") xs

zadanie =  niceformat zaux

main = zadanie

