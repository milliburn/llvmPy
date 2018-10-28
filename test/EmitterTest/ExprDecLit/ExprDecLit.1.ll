%RTAtom = type <{ i64, i64 }>

define void @__body__() {
  %lit = alloca %RTAtom
  store %RTAtom <{ i64 1, i64 1 }>, %RTAtom* %lit
}
