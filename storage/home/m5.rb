# M5Unified Basic Display Example
# Based on actual C extension implementation

require 'm5unified'

puts "Initializing M5Unified..."

# Initialize M5
M5.begin

disp = M5.Display

w = disp.width
h = disp.height
puts "Display: #{w}x#{h}"

# Clear screen to black
disp.fill_screen(0x000000)

# Set text properties and display title
disp.set_text_color(0xFFFFFF)  # White
disp.set_text_size(2)
disp.set_cursor(10, 10)
disp.print("Hello M5!")

# Draw shapes with RGB888 colors
puts "Drawing shapes..."

# Red rectangle
disp.fill_rect(10, 40, 100, 60, 0xFF0000)

# Green circle
disp.fill_circle(w/2, h/2, 40, 0x00FF00)

# Blue line
disp.draw_line(0, 0, w-1, h-1, 0x0000FF)

# Yellow triangle (now implemented!)
disp.fill_triangle(w-80, h-40, w-30, h-60, w-10, h-20, 0xFFFF00)

# Cyan text label
disp.set_text_color(0x00FFFF)
disp.set_text_size(1)
disp.set_cursor(10, h-20)
disp.print("PicoRuby on M5")

puts "Drawing complete!"
puts "Press Ctrl+C to exit"

loop do
  M5.update
  sleep 0.1
end