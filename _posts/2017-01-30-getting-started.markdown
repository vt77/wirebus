---
layout: post
title:  "3 steps to create your own device"
date:   2017-01-30 12:36:05 +0200
category: Getting startet
featured: yes
tags: [Development,Programming]
---
## Getting Started - How to create my own device
In this post we will learn how to create your own device with only 3 simple steps 


## Step 1 - Create project
Wirebus framework supports number of arcitectures and MCUs like AVR: attiny2313,attiny85 PIC: pic12 and STM8 and others. Full list of supported devices you can find in [Supported Devices][devices] page. To create you first wirebus project select architecture and mcu you want. After that in root project directory run  

	./create-project.sh --name=mytest --mcu=pic12f629

Directory `devices/firmware-mytest-pic12f629` will be created for you and project skeleton will be copied to it.
Change to you new-created working directory



## Step 2 - Define wirebus variables


To add new posts, simply add a file in the `_posts` directory that follows the convention `YYYY-MM-DD-name-of-post.ext` and includes the necessary front matter. Take a look at the source for this post to get an idea about how it works.

Jekyll also offers powerful support for code snippets:

## Step 3 - Write the code 

{% highlight c %}
void loop()
{
	//Switch pin according to command
	if(cmd == WIREBUS_DATA_SWITCH)
	{
		if( packet.data	 == 1 )
		{		
			HIGH(SWITCH_PIN);  //Set output pin to high
		}else{
			LOW(SWITCH_PIN);  //Set output pin to low
		}
	}	
}
{% endhighlight %}


## When device firmware created

Check out the [Jekyll docs][jekyll-docs] for more info on how to get the most out of Jekyll. File all bugs/feature requests at [Jekyll’s GitHub repo][jekyll-gh]. If you have questions, you can ask them on [Jekyll Talk][jekyll-talk].

[devices]: /devices/list.html
[jekyll-gh]:   https://github.com/jekyll/jekyll
[jekyll-talk]: https://talk.jekyllrb.com/
