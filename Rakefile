R2P2_ESP32_ROOT = File.dirname(File.expand_path(__FILE__))
MRUBY_ROOT = File.join(R2P2_ESP32_ROOT, "components/picoruby-esp32/picoruby")
$LOAD_PATH << File.join(MRUBY_ROOT, "lib")

# Load environment variables from .env file
if File.exist?(".env")
  File.readlines(".env").each do |line|
    line.strip!
    next if line.empty? || line.start_with?("#")
    key, value = line.split("=", 2)
    ENV[key] = value if key && value
  end
end

# Docker configuration
USE_DOCKER = ENV.fetch("USE_DOCKER", "false") == "true"
USB_SERIAL_PORT = ENV.fetch("USB_SERIAL_PORT", "/dev/ttyUSB0")
BAUD_RATE = ENV.fetch("BAUD_RATE", "115200")
UID = `id -u`.strip
GID = `id -g`.strip
PWD_ = Dir.pwd
ESP_IDF_VERSION = ENV.fetch("ESP_IDF_VERSION", "v5.5.1")
IMAGE = "esp32_build_container:#{ESP_IDF_VERSION}"
DEVICE_ARGS = ENV["DEVICE_ARGS"].to_s
USER_OPT = "--user #{UID}:#{GID}"

DOCKER_CMD = [
  "docker run --rm",
  USER_OPT,
  "-e HOME=/tmp",
  "-v #{PWD_}:/project",
  IMAGE
].join(" ")

DOCKER_CMD_PRIVILEGED = [
  "docker run --rm",
  "--group-add=dialout --group-add=plugdev --privileged",
  DEVICE_ARGS,
  USER_OPT,
  "-e HOME=/tmp",
  "-v #{PWD_}:/project",
  "-v /dev/bus/usb:/dev/bus/usb",
  IMAGE
].join(" ")

DOCKER_CMD_INTERACTIVE = [
  "docker run --rm -it",
  "--group-add=dialout --group-add=plugdev --privileged",
  DEVICE_ARGS,
  USER_OPT,
  "-e HOME=/tmp",
  "-v #{PWD_}:/project",
  "-v /dev/bus/usb:/dev/bus/usb",
  IMAGE
].join(" ")

# Helper method to run commands with optional docker
def run_cmd(cmd, privileged: false, interactive: false)
  if USE_DOCKER
    if interactive
      sh "#{DOCKER_CMD_INTERACTIVE} #{cmd}"
    elsif privileged
      sh "#{DOCKER_CMD_PRIVILEGED} #{cmd}"
    else
      sh "#{DOCKER_CMD} #{cmd}"
    end
  else
    sh cmd
  end
end

# load build systems only when not using Docker
unless USE_DOCKER
  require "mruby/core_ext"
  require "mruby/build"
  require "picoruby/build"

  # load configuration file
  MRUBY_CONFIG = MRuby::Build.mruby_config_path
  load MRUBY_CONFIG
end

desc "Default task that runs all main tasks"
task :default => :all

desc "Build, flash, and monitor the ESP32 project"
task :all => %w[build flash monitor]

desc "Install dependencies and build mruby"
task :setup do
  FileUtils.cd MRUBY_ROOT do
    sh "bundle install"
    sh "rake"
  end
end

%w[esp32 esp32c3 esp32c6 esp32s3].each do |name|
  desc "Setup environment for #{name} target"
  task "setup_#{name}" => %w[deep_clean setup] do
    run_cmd "idf.py set-target #{name}"
  end
end

desc "Build the ESP32 project"
task :build do
  run_cmd "idf.py build"
end

{ picoruby: :mrubyc, microruby: :mruby }.each do |name, vm|
  namespace name do
    desc "Build the ESP32 project with #{name} VM"
    task :build do
      run_cmd "idf.py build -DPICORB_VM=#{vm}"
    end
  end
end

desc "Flash the built firmware to ESP32"
task :flash do
  port_opt = USE_DOCKER ? "-p #{USB_SERIAL_PORT}" : ""
  run_cmd "idf.py #{port_opt} -b #{BAUD_RATE} flash", privileged: true
end

desc "Erase factory partition and flash firmware binary"
task :flash_factory do
  port_opt = USE_DOCKER ? "-p #{USB_SERIAL_PORT}" : ""
  run_cmd "esptool.py #{port_opt} -b #{BAUD_RATE} erase_region 0x10000 0x200000", privileged: true
  run_cmd "esptool.py #{port_opt} -b #{BAUD_RATE} write_flash 0x10000 build/R2P2-ESP32.bin", privileged: true
end

desc "Erase storage partition and flash storage binary"
task :flash_storage do
  port_opt = USE_DOCKER ? "-p #{USB_SERIAL_PORT}" : ""
  run_cmd "esptool.py #{port_opt} -b #{BAUD_RATE} erase_region 0x210000 0x100000", privileged: true
  run_cmd "esptool.py #{port_opt} -b #{BAUD_RATE} write_flash 0x210000 build/storage.bin", privileged: true
end

desc "Monitor ESP32 serial output"
task :monitor do
  port_opt = USE_DOCKER ? "-p #{USB_SERIAL_PORT}" : ""
  run_cmd "idf.py #{port_opt} monitor", privileged: true, interactive: true
end

desc "Clean build artifacts"
task :clean do
  run_cmd "idf.py clean"
  unless USE_DOCKER
    FileUtils.cd MRUBY_ROOT do
      %w[xtensa-esp riscv-esp xtensa-esp-microruby riscv-esp-microruby].each do |mruby_config|
        sh "MRUBY_CONFIG=#{R2P2_ESP32_ROOT}/components/picoruby-esp32/build_config/#{mruby_config}.rb rake clean"
      end
    end
  end
end

desc "Perform deep clean including ESP32 build repos"
task :deep_clean => %w[clean] do
  run_cmd "idf.py fullclean"
  rm_rf File.join(MRUBY_ROOT, "build/repos/esp32")
end

desc "Open menuconfig"
task :menuconfig do
  if USE_DOCKER
    term = ENV['TERM'] || 'xterm-256color'
    docker_cmd_interactive = [
      "docker run --rm -it",
      USER_OPT,
      "-e HOME=/tmp",
      "-e TERM=#{term}",
      "-v #{PWD_}:/project",
      IMAGE
    ].join(" ")
    sh "#{docker_cmd_interactive} idf.py menuconfig"
  else
    sh "idf.py menuconfig"
  end
end

desc "Check ESP32 hardware"
task :check do
  port_opt = USE_DOCKER ? "-p #{USB_SERIAL_PORT}" : ""
  run_cmd "esptool.py #{port_opt} flash_id", privileged: true
end

desc "Generate storage/etc/network/wifi.yml for WiFi auto-connect"
task :gen_wifi_config do
  require 'openssl'
  require 'base64'
  require 'yaml'

  ssid      = ENV['SSID']      or abort "SSID is required. Usage: rake gen_wifi_config SSID=... PASSWORD=... UNIQUE_ID=..."
  password  = ENV['PASSWORD']  or abort "PASSWORD is required."
  unique_id = ENV['UNIQUE_ID'] or abort "UNIQUE_ID is required. Get it from Machine.unique_id on the device."

  # AES-256-CBC encryption (same as wifi_connect.rb decrypt logic)
  key_len = 32
  iv_len  = 16
  len = unique_id.length
  key = (unique_id * ((key_len / len + 1) * len))[0, key_len]
  iv  = (unique_id * ((iv_len  / len + 1) * len))[0, iv_len]

  cipher = OpenSSL::Cipher.new('AES-256-CBC')
  cipher.encrypt
  cipher.key = key
  cipher.iv  = iv
  ciphertext = cipher.update(password) + cipher.final

  encoded_password = Base64.strict_encode64(ciphertext)

  doc = {
    'wifi' => {
      'ssid' => ssid,
      'encoded_password' =>  encoded_password,
      'auto_connect' => (ENV.fetch('AUTO_CONNECT', 'true') == 'true'),
      'retry_if_failed' => (ENV.fetch('RETRY_IF_FAILED', 'true') == 'true'),
      'watchdog' => (ENV.fetch('WATCHDOG', 'false') == 'true')
    }
  }
  doc['country_code'] = ENV['COUNTRY_CODE'] if ENV['COUNTRY_CODE']

  output_path = File.join(R2P2_ESP32_ROOT, "storage/etc/network/wifi.yml")
  FileUtils.mkdir_p(File.dirname(output_path))
  File.write(output_path, YAML.dump(doc).sub(/\A---\s*\n/, ''))
  puts "Generated: #{output_path}"
end
