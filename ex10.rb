args = ""
loop do
  `build/ex10#{args}`
  break unless $?.success?
  args << (" a" * 10000)
  puts "ran with #{args.length/2} arguments"
end

puts "broke with #{args.length/2} arguments"
