using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.SpotifyShell {
	public class RiffHeader {

		public static byte[] create(uint rawLength, uint Freq, uint Chan, uint Bits) {
			byte[] Header = new byte[44];
			
			//Header ID (RIFF)
			insertAt(ref Header, new byte[] { (byte)'R', (byte)'I', (byte)'F', (byte)'F' }, 0);
			//Length of RIFF chunk (Data Length + Header Length - 8)
			insertAt(ref Header, BitConverter.GetBytes((rawLength + 44 - 8)), 4);
			//FourCC Code (WAVE)
			insertAt(ref Header, new byte[] { (byte)'W', (byte)'A', (byte)'V', (byte)'E' }, 8);
			//Format chunk
			insertAt(ref Header, new byte[] { (byte)'f', (byte)'m', (byte)'t', (byte)' ' }, 12);



			//Specify Length (16 Bytes = 0x0010 -> swapped [0x1000])
			insertAt(ref Header, new byte[] { 0x10, 0, 0, 0 }, 16);
			//Specify PCM (0x0001 swapped [0x0100])
			insertAt(ref Header, new byte[] { 1, 0 }, 20);
			//Specify Channels (0x01 or 0x02 (Mono,Stereo))
			insertAt(ref Header, new byte[] { Chan == 2 ? (byte)2 : (byte)1, 0 }, 22);
			//Specify Frequency
			insertAt(ref Header, BitConverter.GetBytes((Freq)), 24);
			//Bytes per Second
			insertAt(ref Header, BitConverter.GetBytes(((uint)(Freq * (Chan == 2 ? 2 : 1) * Bits / 8))), 28);
			//Bytes per Sample
			insertAt(ref Header, BitConverter.GetBytes(((ushort)((Chan == 2 ? 2 : 1) * Bits / 8))), 32);
			//Bits per Sample
			insertAt(ref Header, BitConverter.GetBytes(((ushort)Bits)), 34);



			//"DATA" Chunk
			insertAt(ref Header, new byte[] { (byte)'d', (byte)'a', (byte)'t', (byte)'a' }, 36);
			//Chunk Length
			insertAt(ref Header, BitConverter.GetBytes(rawLength), 40);

			return Header;
		}

		/// <summary>
		/// Generates a RIFF Header with Default Settings (44.1 kHz; 2Channels; 16 Bits)
		/// </summary>
		/// <param name="rawLength">Length (in Bytes) of Data Stream</param>
		/// <returns>RIFF Header for PCM RAW Data</returns>
		public static byte[] create(uint rawLength) {
			return create(rawLength, 44100, 2, 16);
		}

		private static void insertAt(ref byte[] InsertTo, byte[] InsertFrom, int InsertAt) {
			for (int i = 0; i < InsertFrom.Length; i++) {
				InsertTo[InsertAt + i] = InsertFrom[i];
			}
		}

		private static uint swapByteOrder(uint uvalue) {
			return ((0x000000FF) & (uvalue >> 24) |
				(0x0000FF00) & (uvalue >> 8) |
				(0x00FF0000) & (uvalue << 8) |
				(0xFF000000) & (uvalue << 24));
		}

		private static ushort swapByteOrder(ushort uvalue) {
			return (ushort)((0x00FF) & (uvalue >> 8) |
				(0xFF00) & (uvalue << 8));
		}

	}
}
