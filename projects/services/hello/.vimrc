syntax on
set number
set smartindent
set tabstop=4
set shiftwidth=4
set expandtab

let g:netrw_banner = 0
let g:netrw_liststyle = 3
let g:netrw_browse_split = 4
let g:netrw_altv = 1
let g:netrw_winsize = 25
augroup ProjectDrawer
  autocmd!
    autocmd VimEnter * :Vexplore
    augroup END


set mouse=a
