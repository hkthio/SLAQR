
	integer boo
	real dy
	character*40 title, options
	data nx/13/, dx/0.01/, map/1/, boo/1/
	data title/'Nothing here'/
	call setpar()
	call getpar('nx','d',nx)
	call getpar('map','b',map)
	call getpar('dx','f',dx)
	call getpar('title','s',title)
	call getpar('options','s',options)
	call getpar('boo','b',boo)
	call mstpar('dy','f',dy)
	call endpar()
	write(6,100) nx, map, dx, boo, dy
100	format(2x,'nx=',i5,' map=',i3,' dx=',f6.2,' boo=',i3,' dy=',f6.2)
	call pr('title',title)
	call pr('options',options)
c	write(6,101) title, options
c101	format(2x,'title=',40a1,'  options=',40a1)
	stop
	end
