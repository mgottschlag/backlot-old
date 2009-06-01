/*
So wirds nachher benutzt (zum Beispiel, so ungefähr):

AnimationPointer anim = new Animation();
anim->setSize(4, 4);
TexturePointer texture = new Texture();
texture->load("explosion.png");

[...]

texture->bind();
anim->draw(4); // zB Frame 4, später dann auch automatisch, per ->draw();




m: bind() macht dabei nur glBindTexture, den Rest macht draw();
Aber wie weis animation, welche Textur es benutzen soll?
m: Garnicht, das ist von der Textur unabhängig, der zeichnet einfach 0/0 bis 0.25/0.25 von einer beliebigen Textur
Dann muss ich zuerst glEnable(GL_TEXTURE_2D) aufrufen, damit der die richtige benutzt?
m: ne, das glEnable(GL_TEXTURE_2D) passiert in anim->draw()

m: die aktive Textur wird von OpenGL global gespeichert, bis du ne neue festlegst. Genau wie alle anderen Einstellungen.
Aber ich brauch dann trotzdem meherere Animationsobjekte für die ganze Engine? Für jede Animation eine?
m: Theoretisch eins pro Größe, aber man kann ja auch mehr erstellen, bzw wir werden mehr erstellen, schon alleine wegen dem Timer bei ner automatischen Animation, der ist ja immer unterschiedlich. Außerdem isses mit vielen Instanzen einfacher.

m: Aber man könnte das zB auch über ne statische Memberfunktion draw(xtiles, ytiles, frame) machen, die das unabhängig von der Instanz aufruft und von außen immer sichtbar ist, dann kann man Animationen rendern, ohne eine Instanz zu erzeugen.
Aber so kann ich doch nur immer eine Animation abspielen?
m: Wieso?  auch wenn das statisch ist, kann die Funktion ja mehrmals hintereinander mit unterschiedlichen Parametern für die Größe und den Frame aufgerufen werden, und die Textur kann man ja auch immer unterschiedlich setzen.

m: das normale draw() oder draw(frame) rufen dann einfach diese statische Funktion mit der Größe dieser spezifischen Instanz auf.
Aber wenn ich jetzt eine Animation abspiele und während die abgespielt wird, die Textur mit glBind() verändere, dann spielt doch die Animation die neue Textur? m: Aso, ich hab mir das so gedacht, dass draw() immer vom Benutzer manuell aufgerufen wird, der sich dann um Sachen wie Textur und Darstellungsart selber kümmert. Also hab ich zB in "Explosion" eine Textur und eine Animation, und in Explosion::draw() binde ich erst die Textur an OpenGL und dann zeichne ich die Animation (bzw den derzeit aktiven Frame).
Aha. Und was ist Explosion für eine Klasse?
m: Gibts noch nicht, kP
Aber dann brauch ich für alle Sachen, z.B. Einschläge in der Wand und so, eigene Klassen? 
m: Die brauchst du eh, weil alles erst vom Server rübergeschickt wird und dort verwaltet wird. Da gehört ja nicht nur die Animation, sondern auch noch Sound und so dazu, und es gehört dazu, alles auch nach ner bestimmten Zeit wieder aufzuräumen.Also so eine Art class Effect? Genau. Ah, jetzt wird mir das etwas klarer...


*/


/*
Copyright (C) 2009  Simon Kerler

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _ANIMATION_HPP_
#define _ANIMATION_HPP_

#include "Vector2.hpp"
#include "ReferenceCounted.hpp"

#include <vector>

namespace backlot
{
	class Animation : public ReferenceCounted
	{
		public:
			Animation();
			~Animation();

			/**
			 * Set the number of the single frames in a texture. The total number of frames are x*y.
			 * @param x The number fo single frames in x direction.
			 * @param y The number of single frames in y direction.
			 */
			void setFrameNumbers(int x, int y);

			/**
			 * Set the position of the animation.
			 */
			void setPosition(Vector2F position);

			/**
			 * Set the time which the animation needs to be played one time.
			 */
			void setPeriod(unsigned int period);
			
			/**
			 * Set how often the animation is played. -1 for infinite loop.
			 */
			void setLoop(int loopcount);

			/**
			 * If the animation is currently played.
			 */
			bool isPlaying();
			
			/**
			 * Draw one single frame.
			 * @param frame The number of the frame to draw.
			 */
			void draw(int frame);
			/**
			 * Draw the next frame of the animation.
			 */
			bool draw();
			
			void start();
			void stop();
		private:
			Vector2I frames;
			Vector2F sizes;
			Vector2F position;

			unsigned int starttime;
			float frametime;

			int loopcount;
			bool isplayed;

			static std::vector<Animation*> animations;
	};

	typedef SharedPointer<Animation> AnimationPointer;
}

#endif
