#import <Cocoa/Cocoa.h>
#import "CTTabWindowController.h"

@class CTBrowser;
@class CTTabStripController;
@class CTToolbarController;
@class CTTabContents;
@class CTPresentationModeController;

@interface NSDocumentController (CTBrowserWindowControllerAdditions)
- (id)openUntitledDocumentWithWindowController:(NSWindowController*)windowController
                                       display:(BOOL)display
                                         error:(NSError **)outError;
@end

@interface CTBrowserWindowController : CTTabWindowController {
	CTBrowser* browser_; // we own the browser
	CTTabStripController *tabStripController_;
	CTToolbarController *toolbarController_;
@private
	BOOL initializing_; // true if the instance is initializing
	
	// The view that shows the presentation mode toggle when in Lion fullscreen
	// mode.  Nil if not in fullscreen mode or not on Lion.
	NSButton* presentationModeToggleButton_;
	
	// Tracks whether presentation mode was entered from fullscreen mode or
	// directly from normal windowed mode.  Used to determine what to do when
	// exiting presentation mode.
	BOOL enteredPresentationModeFromFullscreen_;
	
	// True between -windowWillEnterFullScreen and -windowDidEnterFullScreen.
	// Only used on Lion.
	BOOL enteringFullscreen_;
	
	CTPresentationModeController* presentationModeController_;
}

@property(strong, readonly, nonatomic) CTTabStripController *tabStripController;
@property(strong, readonly, nonatomic) CTToolbarController *toolbarController;
@property(strong, readonly, nonatomic) CTBrowser *browser;

// Called to check whether or not this window has a toolbar. By default returns
// true if toolbarController_ is not nil.
@property(readonly, nonatomic) BOOL hasToolbar;

//@property(readonly, nonatomic) BOOL shouldShowPresentationModeToggle;

// Returns the current "main" CTBrowserWindowController, or nil if none. "main"
// means the window controller's window is the main window. Useful when there's
// a need to e.g. add contents to the "best window from the users perspective".
+ (CTBrowserWindowController*)mainBrowserWindowController;

// Returns the window controller for |window| or nil if none found
+ (CTBrowserWindowController*)browserWindowControllerForWindow:(NSWindow*)window;

// Returns the window controller for |view| or nil if none found
+ (CTBrowserWindowController*)browserWindowControllerForView:(NSView*)view;

// alias for [[[isa alloc] init] autorelease]
+ (CTBrowserWindowController*)browserWindowController;

- (id)initWithWindowNibPath:(NSString *)windowNibPath
                    browser:(CTBrowser*)browser;
- (id)initWithBrowser:(CTBrowser *)browser;
- (id)init;

// Gets the pattern phase for the window.
- (NSPoint)themePatternPhase;

- (IBAction)saveAllDocuments:(id)sender;
- (IBAction)openDocument:(id)sender;
- (IBAction)newDocument:(id)sender;

// Returns the selected (active) tab, or nil if there are no tabs.
- (CTTabContents*)selectedTabContents;

// Returns the index of the selected (active) tab, or -1 if there are no tabs.
- (int)selectedTabIndex;

// Updates the toolbar with the states of the specified |contents|.
// If |shouldRestore| is true, we're switching (back?) to this tab and should
// restore any previous state (such as user editing a text field) as well.
- (void)updateToolbarWithContents:(CTTabContents*)tab
               shouldRestoreState:(BOOL)shouldRestore;

// Brings this controller's window to the front.
- (void)activate;

// Make the (currently-selected) tab contents the first responder, if possible.
- (void)focusTabContents;

// Lays out the tab content area in the given frame. If the height changes,
// sends a message to the renderer to resize.
- (void)layoutTabContentArea:(NSRect)frame;
@end

// Methods having to do with fullscreen and presentation mode.
@interface CTBrowserWindowController(Fullscreen)
@property(readonly, nonatomic) BOOL isFullscreen; // fullscreen or not

// Returns fullscreen state.  This method is safe to call on all OS versions.
- (BOOL)isFullscreen;

// Whether to show the presentation mode toggle button in the UI.  Returns YES
// if in fullscreen mode on Lion or later.  This method is safe to call on all
// OS versions.
// Subclasses can overwrite this to decide whether or not show presentation 
// mode toggle button.
- (BOOL)shouldShowPresentationModeToggle;

// Resizes the fullscreen window to fit the screen it's currently on.  Called by
// the PresentationModeController when there is a change in monitor placement or
// resolution.
- (void)resizeFullscreenWindow;

// Gets or sets the fraction of the floating bar (presentation mode overlay)
// that is shown.  0 is completely hidden, 1 is fully shown.
- (CGFloat)floatingBarShownFraction;
- (void)setFloatingBarShownFraction:(CGFloat)fraction;

// Query/lock/release the requirement that the tab strip/toolbar/attached
// bookmark bar bar cluster is visible (e.g., when one of its elements has
// focus). This is required for the floating bar in presentation mode, but
// should also be called when not in presentation mode; see the comments for
// |barVisibilityLocks_| for more details. Double locks/releases by the same
// owner are ignored. If |animate:| is YES, then an animation may be performed,
// possibly after a small delay if |delay:| is YES. If |animate:| is NO,
// |delay:| will be ignored. In the case of multiple calls, later calls have
// precedence with the rule that |animate:NO| has precedence over |animate:YES|,
// and |delay:NO| has precedence over |delay:YES|.
- (BOOL)isBarVisibilityLockedForOwner:(id)owner;
- (void)lockBarVisibilityForOwner:(id)owner
                    withAnimation:(BOOL)animate
                            delay:(BOOL)delay;
- (void)releaseBarVisibilityForOwner:(id)owner
                       withAnimation:(BOOL)animate
                               delay:(BOOL)delay;
@end  // @interface BrowserWindowController(Fullscreen)
