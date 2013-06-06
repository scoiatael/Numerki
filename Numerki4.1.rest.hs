
zadupnew = upnewton f fp 0.0
zadsiecz = map (\(a,b) -> (a+b)/2) $ siecznych f 0.75 0.0
zadfalsi = map (\(a,b) -> (a+b)/2) $ falsi f 0 0.75

zip4 (x:xs) (y:ys) (w:ws) (z:zs) = [x,y,w,z]:(zip4 xs ys ws zs)

zaux = takeWhile (\xs -> and (map (\x -> abs((log x)/(log 10)) < 7) xs)) $ map (map (\x -> abs(x-alpha))) (zip4 zadnew zadupnew zadsiecz zadfalsi)


